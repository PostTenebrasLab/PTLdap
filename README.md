## PTLdap

A simple (over-engineered) LDAP and BER C++17 library

### Preparing the build environment

Retrieve the submodule for the TCP library:

    git submodule update --init

Prepare the build folder:

    mkdir -p build/
    cd build/
    cmake ..
    # Or for using Ninja
    cmake -G "Ninja" ..

### Running the tests and examples

#### Building with make

    cd build/
    # make <target>
    make example_ldap_search
    make tests

#### Building with ninja

    cd build/
    # ninja <target>
    ninja example_ldap_search
    ninja tests