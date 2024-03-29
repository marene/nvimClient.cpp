const nvimTypesMapping = Object.freeze({
    Nil: 'packer::Void',
    Void: 'packer::Void',
    void: 'packer::Void',
    Boolean: 'bool',
    Integer: 'int64_t',
    Float: 'double',
    String: 'std::string',
    Dictionary: 'nvimRpc::types::Dictionary',
    Object: 'msgpack::type::ext',
    Array: 'nvimRpc::types::Array',
});
const nvimDefaultTypeMapping = 'msgpack::type::ext'; 

const arrayOfRegexp = /ArrayOf\(([^,\)]+)(,\s*\d+)?\)/;

function cppVector(type) {
    return `std::vector<${type}>`;
}

function getFormattedType(type) {
    const arrayOfMatch = type.match(arrayOfRegexp);

    if (arrayOfMatch) {
        return cppVector(getFormattedType(arrayOfMatch[1]));
    }

    return nvimTypesMapping[type] || nvimDefaultTypeMapping;
}

module.exports = {
    getFormattedType,
};
