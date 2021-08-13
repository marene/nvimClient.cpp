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

function listParametersTypes(functionParams) {
    const paramsList = functionParams.map(
        functionParam => functionParam.type,
    );

    return paramsList.join(', ');}

function getFunctionHeader(fnType, fnName, fnParams) {
    return `std::future<${fnType}> ${fnName}(${listParameters(fnParams, true)})`
}

function getFunctionImplementation(fnName, fnType, fnParams) {
    const listedParams = listParameters(fnParams);
    const listedParamsTypes = listParametersTypes(fnParams);

    return `\
auto packedRequest = _packRequest("${fnName}"${listedParams.length ? ', ' + listedParams : ''});


return _dispatcher->placeCall<${fnType}${listedParamsTypes.length ? ', ' + listedParamsTypes : ''}>(packedRequest);
`;
}

function defineFunctions(apiInfo) {
    let functions = '';
    apiInfo.functions.forEach(fn => {
        if (fn.deprecated_since && fn.deprecated_since <= apiInfo.version.api_level) {
            return;
        }
        const fnType = getFormattedType(fn.return_type);
        const fnParams = getFunctionParameters(fn.parameters);
        functions += `

${getFunctionHeader(fnType, fn.name, fnParams)} {
    ${getFunctionImplementation(fn.name, fnType, fnParams)}
}
`;
    });

    return functions
}

module.exports = {
    defineFunctions
};
