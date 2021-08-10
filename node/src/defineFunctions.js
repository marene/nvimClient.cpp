const { getFormattedType } = require('./types');

function getFunctionParameters(fnParams) {
    return fnParams.map(fnParam => ({
        name: fnParam[1],
        type: getFormattedType(fnParam[0]),
    }));
}

function listParameters(functionParams, includeParamsTypes = false) {
    const paramsList = functionParams.map(
        functionParam =>
        `${includeParamsTypes? functionParam.type + ' ' : ''}${functionParam.name}`
    );

    return paramsList.join(', ');
}

function getFunctionHeader(fnType, fnName, fnParams) {
    return `${fnType} ${fnName}(${listParameters(fnParams, true)})`
}

function getFunctionImplementation(fnName, fnType, fnParams) {
    const isFnTypeVoid = fnType === 'void';
    const callType = isFnTypeVoid ? 'packer::Void' : fnType;
    const fnEnd = isFnTypeVoid ? '' : 'return ret;'
    const listedParams = listParameters(fnParams);
    let fnImplementation = '';

    if (!isFnTypeVoid) {
        fnImplementation += `${fnType} ret;\n`
    }

    return fnImplementation + `\
auto packedResponse = _call<${callType}>("${fnName}"${listedParams.length ? ', ' + listedParams : ''});

_handleResponse(packedResponse${isFnTypeVoid ? '' : ', ret'});

`  + fnEnd;
}

function defineFunctions(headerFile, apiInfo) {
    apiInfo.functions.forEach(fn => {
        const fnType = getFormattedType(fn.return_type);
        const fnParams = getFunctionParameters(fn.parameters);
        headerFile.write(`

${getFunctionHeader(fnType, fn.name, fnParams)} {
    ${getFunctionImplementation(fn.name, fnType, fnParams)}
}
`);
    });
}

module.exports = {
    defineFunctions
};
