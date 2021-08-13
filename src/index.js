const fs = require('fs');
const msgpack = require('msgpack');
const { defineFunctions } = require('./defineFunctions');
const { headerSetup, headerConclude } = require('./headerSetup');

function generateHeader(unpackedApiInfo) {
    let headerFile = '';
    headerFile += headerSetup();
    headerFile += defineFunctions(unpackedApiInfo);
    headerFile += headerConclude();

    return headerFile;
}

async function main(apiInfoFile) {
    const apiInfoBuffer = fs.readFileSync(apiInfoFile);
    const unpackedApiInfo = msgpack.unpack(apiInfoBuffer);

    return generateHeader(unpackedApiInfo);
}

main(process.argv[2])
    .then(header => {
        console.log(header);
    })
    .catch(err => {
        console.error('An error occured: ', err);
    });
