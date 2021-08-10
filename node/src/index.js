const fs = require('fs');
const msgpack = require('msgpack');
const { defineFunctions } = require('./defineFunctions');
const { headerSetup, headerConclude } = require('./headerSetup');

function generateHeader(unpackedApiInfo, headerFile) {
    headerSetup(headerFile);
    defineFunctions(headerFile, unpackedApiInfo);
    headerConclude(headerFile);
}

async function main(apiInfoFile) {
    const apiInfoBuffer = fs.readFileSync(apiInfoFile);
    const unpackedApiInfo = msgpack.unpack(apiInfoBuffer);
    const headerFile = fs.createWriteStream('./generated_header.hpp');

    try {
        generateHeader(unpackedApiInfo, headerFile);
    } finally {
        headerFile.close();
    }

    return unpackedApiInfo;
}

main('/tmp/apiinfo')
    .then(() => {
        console.log("Unpacked with success.");
    })
    .catch(err => {
        console.error('An error occured: ', err);
    });
