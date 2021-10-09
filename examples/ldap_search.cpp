#include "base_client.hpp"

#include "ldap.hpp"


int main(int argc, char** argv) {
    // Conenct to the LDAP server
    const auto ip = "10.42.129.204";
    const auto port = 389;

    auto client = LDAPClient(ip, port);
    if (client.connection == nullptr) {
        printf("Unable to connect!\n");
        exit(1);
    }

    // Prepare a message to send
    auto writer = Bytes::StringWriter();
    LDAP::message(
        0x02,
        LDAP::search_request(
            "dc=posttenebraslab,dc=ch"sv,
            LDAP::SearchRequestScope::WholeSubtree,
            LDAP::SearchRequestDerefAliases::NeverDerefAliases,
            1000, 30, false,
            LDAP::filter.make<LDAP::Filter::And>(
                LDAP::filter.make<LDAP::Filter::EqualityMatch>(
                    "uid"sv,
                    "mborges"sv
                ),
                LDAP::filter.make<LDAP::Filter::EqualityMatch>(
                    "givenName"sv,
                    "Maxime"sv
                )
            ),
            LDAP::attribute_selection("cn"sv)
        ),
        std::nullopt
    ).write(writer);

    // Prepare the response handler
    client.connection->setDataCallback([](brynet::base::BasePacketReader& reader) {
        auto ldap_reader = BrynetLdapReader{reader};

        while(!ldap_reader.empty()) {
            auto res = LDAP::message.read(ldap_reader);
            if (!res.has_value()) {
                printf("Not a LDAP message!\n");
                return;
            }

            auto [message_id, protocol_op, controls_opt] = res.value();

            bool found = false;

            switch (protocol_op.tag_number) {
                case LDAP::ProtocolOp::SearchResultEntry: {
                    auto [object_name, attributes] = protocol_op.get<LDAP::ProtocolOp::SearchResultEntry>();
                    printf("Object name: %.*s\n", object_name.size(), object_name.data());
                    while (!attributes.empty()) {
                        auto partial_attribute = LDAP::partial_attribute.read(attributes);
                        if (!partial_attribute) {
                            printf("Expected partial attribute\n");
                            exit(1);
                        }
                        auto [type, vals] = *partial_attribute;
                        printf("Attribute type: %.*s\n", type.size(), type.data());
                        while (!vals.empty()) {
                            auto val = LDAP::attribute_value.read(vals);
                            if (!val) {
                                printf("Expected attribute value\n");
                                exit(1);
                            }
                            printf("Attribute value: %.*s\n", val->size(), val->data());
                        }
                    }
                    continue;
                }
                case LDAP::ProtocolOp::SearchResultDone: {
                    auto [result_code, matched_dn, diagnostic_message, referral] = protocol_op.get<LDAP::ProtocolOp::SearchResultDone>();
                    switch (result_code) {
                    case LDAP::ResultCode::Success:
                    case LDAP::ResultCode::SizeLimitExceeded:
                        printf("Search finished, exiting.\n");
                        exit(0);
                    default:
                        printf("Expected search response success, got %d: %s\n", result_code, diagnostic_message);
                    }
                }
                default:

                    printf("Expected search response, got %d\n", protocol_op.tag_number);
            }
        }
    });
    // Send the message
    client.connection->send(writer.string);

    // Let the async client handle the responses
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (brynet::base::app_kbhit())
            break;
    }
    
    return 0;
}