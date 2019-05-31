//
//  parser_configuration.h
//  blocksci_devel
//
//  Created by Harry Kalodner on 3/14/17.
//  Copyright © 2017 Harry Kalodner. All rights reserved.
//

#include "parser_configuration.hpp"
#include <blocksci/util/hash.hpp>

#ifdef BLOCKSCI_RPC_PARSER
#include <bitcoinapi/bitcoinapi.h>
#endif

#include <boost/filesystem/operations.hpp>

ParserConfigurationBase::ParserConfigurationBase() : dataConfig() {}

ParserConfigurationBase::ParserConfigurationBase(const std::string &dataDirectory_) : dataConfig(dataDirectory_) {
    boost::filesystem::path dir(parserDirectory());
    
    if(!(boost::filesystem::exists(dir))){
        boost::filesystem::create_directory(dir);
    }
    
    if(!(boost::filesystem::exists(addressPath()))){
        boost::filesystem::create_directory(addressPath());
    }
    
    if(!(boost::filesystem::exists(utxoAddressStatePath()))){
        boost::filesystem::create_directory(utxoAddressStatePath());
    }
}

#ifdef BLOCKSCI_FILE_PARSER
ParserConfiguration<FileTag>::ParserConfiguration() : ParserConfigurationBase() {

}

ParserConfiguration<FileTag>::ParserConfiguration(boost::filesystem::path bitcoinDirectory_, const std::string &dataDirectory_) : ParserConfigurationBase(dataDirectory_), bitcoinDirectory(std::move(bitcoinDirectory_)) {
    auto bitcoinDirectoryString = bitcoinDirectory.native();
    if (bitcoinDirectoryString.find("litecoin") != std::string::npos || bitcoinDirectoryString.find("Litecoin") != std::string::npos) {
        blockMagic = 0xdbb6c0fb;
        workHashFunction = doubleSha256;
    } else if (bitcoinDirectoryString.find("regtest") != std::string::npos) {
        blockMagic = 0xdab5bffa;
        workHashFunction = doubleSha256;
    } else if (bitcoinDirectoryString.find("testnet") != std::string::npos) {
        blockMagic = 0x0709110b;
        workHashFunction = doubleSha256;
    } else {
        blockMagic = 0xd9b4bef9;
        workHashFunction = doubleSha256;
    }
}


boost::filesystem::path ParserConfiguration<FileTag>::pathForBlockFile(int fileNum) const {
    std::ostringstream convert;
    convert << fileNum;
    std::string numString = convert.str();
    return (bitcoinDirectory/"blocks"/"blk").concat(std::string(5 - numString.size(), '0')).concat(numString).concat(".dat");
}
#endif

#ifdef BLOCKSCI_RPC_PARSER
ParserConfiguration<RPCTag>::ParserConfiguration() : ParserConfigurationBase() {}

ParserConfiguration<RPCTag>::ParserConfiguration(std::string username_, std::string password_, std::string address_, int port_, const std::string &dataDirectory_) : ParserConfigurationBase(dataDirectory_), username(std::move(username_)), password(std::move(password_)), address(std::move(address_)), port(port_) {}


BitcoinAPI ParserConfiguration<RPCTag>::createBitcoinAPI() const {
    return BitcoinAPI{username, password, address, port};
}
#endif

