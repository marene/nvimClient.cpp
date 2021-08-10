
#ifndef NVIM_CLIENT
#define NVIM_CLIENT
#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "impl/MsgPacker.hpp"
#include "impl/TcpConnector.hpp"
#include "impl/types.hpp"
#include "msgpack.hpp"

namespace nvimRpc {
struct ClientConfig {
    std::string host;
    int port;
};

class ClientError : std::exception {
  private:
    std::string _errorMessage;

  public:
    ClientError(const packer::Error& error) { _errorMessage = std::string(error.get<1>()); };

    const char* what() const noexcept { return _errorMessage.data(); };
};

class Client {
  private:
    Tcp::Connector* _connector;
    uint64_t _msgid;

    template <typename T, typename... U>
    packer::PackedRequestResponse<T> _call(const std::string& method, const U&... args) {
        packer::PackedRequest<U...> packedRequest(method, _msgid++, args...);

        _connector->send(packedRequest.data(), packedRequest.size());
        std::vector<char> rawApiRes = _connector->read();

        return packer::PackedRequestResponse<T>(rawApiRes);
    };

    template <typename T> void _handleResponse(const packer::PackedRequestResponse<T>& response, T& ret) {
        if (response.error()) {
            throw ClientError(*(response.error()));
        }
        ret = *(response.value());
    }

    template <typename T> void _handleResponse(const packer::PackedRequestResponse<T>& response) {
        if (response.error()) {
            throw ClientError(*(response.error()));
        }
    }

  public:
    Client(Tcp::Connector* connector) {
        this->_connector = connector;
        this->_msgid = 0;
    };

    void connect() { _connector->connect(); };

    int64_t nvim_buf_line_count(msgpack::type::ext buffer) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_buf_line_count", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool nvim_buf_attach(msgpack::type::ext buffer, bool send_buffer, nvimRpc::types::Dictionary opts) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_buf_attach", buffer, send_buffer, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool nvim_buf_detach(msgpack::type::ext buffer) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_buf_detach", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<std::string> nvim_buf_get_lines(msgpack::type::ext buffer, int64_t start, int64_t end,
                                                bool strict_indexing) {
        std::vector<std::string> ret;
        auto packedResponse =
            _call<std::vector<std::string>>("nvim_buf_get_lines", buffer, start, end, strict_indexing);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_set_lines(msgpack::type::ext buffer, int64_t start, int64_t end, bool strict_indexing,
                            std::vector<std::string> replacement) {
        auto packedResponse =
            _call<packer::Void>("nvim_buf_set_lines", buffer, start, end, strict_indexing, replacement);

        _handleResponse(packedResponse);
    }

    void nvim_buf_set_text(msgpack::type::ext buffer, int64_t start_row, int64_t start_col, int64_t end_row,
                           int64_t end_col, std::vector<std::string> replacement) {
        auto packedResponse =
            _call<packer::Void>("nvim_buf_set_text", buffer, start_row, start_col, end_row, end_col, replacement);

        _handleResponse(packedResponse);
    }

    int64_t nvim_buf_get_offset(msgpack::type::ext buffer, int64_t index) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_buf_get_offset", buffer, index);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_buf_get_var(msgpack::type::ext buffer, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_buf_get_var", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_buf_get_changedtick(msgpack::type::ext buffer) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_buf_get_changedtick", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<nvimRpc::types::Dictionary> nvim_buf_get_keymap(msgpack::type::ext buffer, std::string mode) {
        std::vector<nvimRpc::types::Dictionary> ret;
        auto packedResponse = _call<std::vector<nvimRpc::types::Dictionary>>("nvim_buf_get_keymap", buffer, mode);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_set_keymap(msgpack::type::ext buffer, std::string mode, std::string lhs, std::string rhs,
                             nvimRpc::types::Dictionary opts) {
        auto packedResponse = _call<packer::Void>("nvim_buf_set_keymap", buffer, mode, lhs, rhs, opts);

        _handleResponse(packedResponse);
    }

    void nvim_buf_del_keymap(msgpack::type::ext buffer, std::string mode, std::string lhs) {
        auto packedResponse = _call<packer::Void>("nvim_buf_del_keymap", buffer, mode, lhs);

        _handleResponse(packedResponse);
    }

    nvimRpc::types::Dictionary nvim_buf_get_commands(msgpack::type::ext buffer, nvimRpc::types::Dictionary opts) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_buf_get_commands", buffer, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_set_var(msgpack::type::ext buffer, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_buf_set_var", buffer, name, value);

        _handleResponse(packedResponse);
    }

    void nvim_buf_del_var(msgpack::type::ext buffer, std::string name) {
        auto packedResponse = _call<packer::Void>("nvim_buf_del_var", buffer, name);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_buf_get_option(msgpack::type::ext buffer, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_buf_get_option", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_set_option(msgpack::type::ext buffer, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_buf_set_option", buffer, name, value);

        _handleResponse(packedResponse);
    }

    std::string nvim_buf_get_name(msgpack::type::ext buffer) {
        std::string ret;
        auto packedResponse = _call<std::string>("nvim_buf_get_name", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_set_name(msgpack::type::ext buffer, std::string name) {
        auto packedResponse = _call<packer::Void>("nvim_buf_set_name", buffer, name);

        _handleResponse(packedResponse);
    }

    bool nvim_buf_is_loaded(msgpack::type::ext buffer) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_buf_is_loaded", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_delete(msgpack::type::ext buffer, nvimRpc::types::Dictionary opts) {
        auto packedResponse = _call<packer::Void>("nvim_buf_delete", buffer, opts);

        _handleResponse(packedResponse);
    }

    bool nvim_buf_is_valid(msgpack::type::ext buffer) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_buf_is_valid", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<int64_t> nvim_buf_get_mark(msgpack::type::ext buffer, std::string name) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("nvim_buf_get_mark", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<int64_t> nvim_buf_get_extmark_by_id(msgpack::type::ext buffer, int64_t ns_id, int64_t id,
                                                    nvimRpc::types::Dictionary opts) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("nvim_buf_get_extmark_by_id", buffer, ns_id, id, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array nvim_buf_get_extmarks(msgpack::type::ext buffer, int64_t ns_id, msgpack::type::ext start,
                                                msgpack::type::ext end, nvimRpc::types::Dictionary opts) {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("nvim_buf_get_extmarks", buffer, ns_id, start, end, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_buf_set_extmark(msgpack::type::ext buffer, int64_t ns_id, int64_t line, int64_t col,
                                 nvimRpc::types::Dictionary opts) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_buf_set_extmark", buffer, ns_id, line, col, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool nvim_buf_del_extmark(msgpack::type::ext buffer, int64_t ns_id, int64_t id) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_buf_del_extmark", buffer, ns_id, id);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_buf_add_highlight(msgpack::type::ext buffer, int64_t ns_id, std::string hl_group, int64_t line,
                                   int64_t col_start, int64_t col_end) {
        int64_t ret;
        auto packedResponse =
            _call<int64_t>("nvim_buf_add_highlight", buffer, ns_id, hl_group, line, col_start, col_end);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_clear_namespace(msgpack::type::ext buffer, int64_t ns_id, int64_t line_start, int64_t line_end) {
        auto packedResponse = _call<packer::Void>("nvim_buf_clear_namespace", buffer, ns_id, line_start, line_end);

        _handleResponse(packedResponse);
    }

    int64_t nvim_buf_set_virtual_text(msgpack::type::ext buffer, int64_t src_id, int64_t line,
                                      nvimRpc::types::Array chunks, nvimRpc::types::Dictionary opts) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_buf_set_virtual_text", buffer, src_id, line, chunks, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_buf_call(msgpack::type::ext buffer, msgpack::type::ext fun) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_buf_call", buffer, fun);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::string nvim_command_output(std::string command) {
        std::string ret;
        auto packedResponse = _call<std::string>("nvim_command_output", command);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_execute_lua(std::string code, nvimRpc::types::Array args) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_execute_lua", code, args);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_buf_get_number(msgpack::type::ext buffer) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_buf_get_number", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_buf_clear_highlight(msgpack::type::ext buffer, int64_t ns_id, int64_t line_start, int64_t line_end) {
        auto packedResponse = _call<packer::Void>("nvim_buf_clear_highlight", buffer, ns_id, line_start, line_end);

        _handleResponse(packedResponse);
    }

    void buffer_insert(msgpack::type::ext buffer, int64_t lnum, std::vector<std::string> lines) {
        auto packedResponse = _call<packer::Void>("buffer_insert", buffer, lnum, lines);

        _handleResponse(packedResponse);
    }

    std::string buffer_get_line(msgpack::type::ext buffer, int64_t index) {
        std::string ret;
        auto packedResponse = _call<std::string>("buffer_get_line", buffer, index);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void buffer_set_line(msgpack::type::ext buffer, int64_t index, std::string line) {
        auto packedResponse = _call<packer::Void>("buffer_set_line", buffer, index, line);

        _handleResponse(packedResponse);
    }

    void buffer_del_line(msgpack::type::ext buffer, int64_t index) {
        auto packedResponse = _call<packer::Void>("buffer_del_line", buffer, index);

        _handleResponse(packedResponse);
    }

    std::vector<std::string> buffer_get_line_slice(msgpack::type::ext buffer, int64_t start, int64_t end,
                                                   bool include_start, bool include_end) {
        std::vector<std::string> ret;
        auto packedResponse =
            _call<std::vector<std::string>>("buffer_get_line_slice", buffer, start, end, include_start, include_end);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void buffer_set_line_slice(msgpack::type::ext buffer, int64_t start, int64_t end, bool include_start,
                               bool include_end, std::vector<std::string> replacement) {
        auto packedResponse =
            _call<packer::Void>("buffer_set_line_slice", buffer, start, end, include_start, include_end, replacement);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext buffer_set_var(msgpack::type::ext buffer, std::string name, msgpack::type::ext value) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("buffer_set_var", buffer, name, value);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext buffer_del_var(msgpack::type::ext buffer, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("buffer_del_var", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext window_set_var(msgpack::type::ext window, std::string name, msgpack::type::ext value) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("window_set_var", window, name, value);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext window_del_var(msgpack::type::ext window, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("window_del_var", window, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext tabpage_set_var(msgpack::type::ext tabpage, std::string name, msgpack::type::ext value) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("tabpage_set_var", tabpage, name, value);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext tabpage_del_var(msgpack::type::ext tabpage, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("tabpage_del_var", tabpage, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_set_var(std::string name, msgpack::type::ext value) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_set_var", name, value);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_del_var(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_del_var", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<msgpack::type::ext> nvim_tabpage_list_wins(msgpack::type::ext tabpage) {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("nvim_tabpage_list_wins", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_tabpage_get_var(msgpack::type::ext tabpage, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_tabpage_get_var", tabpage, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_tabpage_set_var(msgpack::type::ext tabpage, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_tabpage_set_var", tabpage, name, value);

        _handleResponse(packedResponse);
    }

    void nvim_tabpage_del_var(msgpack::type::ext tabpage, std::string name) {
        auto packedResponse = _call<packer::Void>("nvim_tabpage_del_var", tabpage, name);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_tabpage_get_win(msgpack::type::ext tabpage) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_tabpage_get_win", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_tabpage_get_number(msgpack::type::ext tabpage) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_tabpage_get_number", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool nvim_tabpage_is_valid(msgpack::type::ext tabpage) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_tabpage_is_valid", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_ui_attach(int64_t width, int64_t height, nvimRpc::types::Dictionary options) {
        auto packedResponse = _call<packer::Void>("nvim_ui_attach", width, height, options);

        _handleResponse(packedResponse);
    }

    void ui_attach(int64_t width, int64_t height, bool enable_rgb) {
        auto packedResponse = _call<packer::Void>("ui_attach", width, height, enable_rgb);

        _handleResponse(packedResponse);
    }

    void nvim_ui_detach() {
        auto packedResponse = _call<packer::Void>("nvim_ui_detach");

        _handleResponse(packedResponse);
    }

    void nvim_ui_try_resize(int64_t width, int64_t height) {
        auto packedResponse = _call<packer::Void>("nvim_ui_try_resize", width, height);

        _handleResponse(packedResponse);
    }

    void nvim_ui_set_option(std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_ui_set_option", name, value);

        _handleResponse(packedResponse);
    }

    void nvim_ui_try_resize_grid(int64_t grid, int64_t width, int64_t height) {
        auto packedResponse = _call<packer::Void>("nvim_ui_try_resize_grid", grid, width, height);

        _handleResponse(packedResponse);
    }

    void nvim_ui_pum_set_height(int64_t height) {
        auto packedResponse = _call<packer::Void>("nvim_ui_pum_set_height", height);

        _handleResponse(packedResponse);
    }

    void nvim_ui_pum_set_bounds(double width, double height, double row, double col) {
        auto packedResponse = _call<packer::Void>("nvim_ui_pum_set_bounds", width, height, row, col);

        _handleResponse(packedResponse);
    }

    std::string nvim_exec(std::string src, bool output) {
        std::string ret;
        auto packedResponse = _call<std::string>("nvim_exec", src, output);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_command(std::string command) {
        auto packedResponse = _call<packer::Void>("nvim_command", command);

        _handleResponse(packedResponse);
    }

    nvimRpc::types::Dictionary nvim_get_hl_by_name(std::string name, bool rgb) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_hl_by_name", name, rgb);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_hl_by_id(int64_t hl_id, bool rgb) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_hl_by_id", hl_id, rgb);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_get_hl_id_by_name(std::string name) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_get_hl_id_by_name", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_hl(int64_t ns_id, std::string name, nvimRpc::types::Dictionary val) {
        auto packedResponse = _call<packer::Void>("nvim_set_hl", ns_id, name, val);

        _handleResponse(packedResponse);
    }

    void nvim_feedkeys(std::string keys, std::string mode, bool escape_csi) {
        auto packedResponse = _call<packer::Void>("nvim_feedkeys", keys, mode, escape_csi);

        _handleResponse(packedResponse);
    }

    int64_t nvim_input(std::string keys) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_input", keys);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_input_mouse(std::string button, std::string action, std::string modifier, int64_t grid, int64_t row,
                          int64_t col) {
        auto packedResponse = _call<packer::Void>("nvim_input_mouse", button, action, modifier, grid, row, col);

        _handleResponse(packedResponse);
    }

    std::string nvim_replace_termcodes(std::string str, bool from_part, bool do_lt, bool special) {
        std::string ret;
        auto packedResponse = _call<std::string>("nvim_replace_termcodes", str, from_part, do_lt, special);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_eval(std::string expr) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_eval", expr);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_exec_lua(std::string code, nvimRpc::types::Array args) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_exec_lua", code, args);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_notify(std::string msg, int64_t log_level, nvimRpc::types::Dictionary opts) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_notify", msg, log_level, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_call_function(std::string fn, nvimRpc::types::Array args) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_call_function", fn, args);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_call_dict_function(msgpack::type::ext dict, std::string fn, nvimRpc::types::Array args) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_call_dict_function", dict, fn, args);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_strwidth(std::string text) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_strwidth", text);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<std::string> nvim_list_runtime_paths() {
        std::vector<std::string> ret;
        auto packedResponse = _call<std::vector<std::string>>("nvim_list_runtime_paths");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<std::string> nvim_get_runtime_file(std::string name, bool all) {
        std::vector<std::string> ret;
        auto packedResponse = _call<std::vector<std::string>>("nvim_get_runtime_file", name, all);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_current_dir(std::string dir) {
        auto packedResponse = _call<packer::Void>("nvim_set_current_dir", dir);

        _handleResponse(packedResponse);
    }

    std::string nvim_get_current_line() {
        std::string ret;
        auto packedResponse = _call<std::string>("nvim_get_current_line");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_current_line(std::string line) {
        auto packedResponse = _call<packer::Void>("nvim_set_current_line", line);

        _handleResponse(packedResponse);
    }

    void nvim_del_current_line() {
        auto packedResponse = _call<packer::Void>("nvim_del_current_line");

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_get_var(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_var", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_var(std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_set_var", name, value);

        _handleResponse(packedResponse);
    }

    void nvim_del_var(std::string name) {
        auto packedResponse = _call<packer::Void>("nvim_del_var", name);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_get_vvar(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_vvar", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_vvar(std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_set_vvar", name, value);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_get_option(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_option", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_all_options_info() {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_all_options_info");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_option_info(std::string name) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_option_info", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_option(std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_set_option", name, value);

        _handleResponse(packedResponse);
    }

    void nvim_echo(nvimRpc::types::Array chunks, bool history, nvimRpc::types::Dictionary opts) {
        auto packedResponse = _call<packer::Void>("nvim_echo", chunks, history, opts);

        _handleResponse(packedResponse);
    }

    void nvim_out_write(std::string str) {
        auto packedResponse = _call<packer::Void>("nvim_out_write", str);

        _handleResponse(packedResponse);
    }

    void nvim_err_write(std::string str) {
        auto packedResponse = _call<packer::Void>("nvim_err_write", str);

        _handleResponse(packedResponse);
    }

    void nvim_err_writeln(std::string str) {
        auto packedResponse = _call<packer::Void>("nvim_err_writeln", str);

        _handleResponse(packedResponse);
    }

    std::vector<msgpack::type::ext> nvim_list_bufs() {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("nvim_list_bufs");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_get_current_buf() {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_current_buf");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_current_buf(msgpack::type::ext buffer) {
        auto packedResponse = _call<packer::Void>("nvim_set_current_buf", buffer);

        _handleResponse(packedResponse);
    }

    std::vector<msgpack::type::ext> nvim_list_wins() {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("nvim_list_wins");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_get_current_win() {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_current_win");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_current_win(msgpack::type::ext window) {
        auto packedResponse = _call<packer::Void>("nvim_set_current_win", window);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_create_buf(bool listed, bool scratch) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_create_buf", listed, scratch);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_open_term(msgpack::type::ext buffer, nvimRpc::types::Dictionary opts) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_open_term", buffer, opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_chan_send(int64_t chan, std::string data) {
        auto packedResponse = _call<packer::Void>("nvim_chan_send", chan, data);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_open_win(msgpack::type::ext buffer, bool enter, nvimRpc::types::Dictionary config) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_open_win", buffer, enter, config);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<msgpack::type::ext> nvim_list_tabpages() {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("nvim_list_tabpages");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_get_current_tabpage() {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_current_tabpage");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_current_tabpage(msgpack::type::ext tabpage) {
        auto packedResponse = _call<packer::Void>("nvim_set_current_tabpage", tabpage);

        _handleResponse(packedResponse);
    }

    int64_t nvim_create_namespace(std::string name) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_create_namespace", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_namespaces() {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_namespaces");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool nvim_paste(std::string data, bool crlf, int64_t phase) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_paste", data, crlf, phase);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_put(std::vector<std::string> lines, std::string type, bool after, bool follow) {
        auto packedResponse = _call<packer::Void>("nvim_put", lines, type, after, follow);

        _handleResponse(packedResponse);
    }

    void nvim_subscribe(std::string event) {
        auto packedResponse = _call<packer::Void>("nvim_subscribe", event);

        _handleResponse(packedResponse);
    }

    void nvim_unsubscribe(std::string event) {
        auto packedResponse = _call<packer::Void>("nvim_unsubscribe", event);

        _handleResponse(packedResponse);
    }

    int64_t nvim_get_color_by_name(std::string name) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_get_color_by_name", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_color_map() {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_color_map");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_context(nvimRpc::types::Dictionary opts) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_context", opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_load_context(nvimRpc::types::Dictionary dict) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_load_context", dict);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_get_mode() {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_mode");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<nvimRpc::types::Dictionary> nvim_get_keymap(std::string mode) {
        std::vector<nvimRpc::types::Dictionary> ret;
        auto packedResponse = _call<std::vector<nvimRpc::types::Dictionary>>("nvim_get_keymap", mode);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_keymap(std::string mode, std::string lhs, std::string rhs, nvimRpc::types::Dictionary opts) {
        auto packedResponse = _call<packer::Void>("nvim_set_keymap", mode, lhs, rhs, opts);

        _handleResponse(packedResponse);
    }

    void nvim_del_keymap(std::string mode, std::string lhs) {
        auto packedResponse = _call<packer::Void>("nvim_del_keymap", mode, lhs);

        _handleResponse(packedResponse);
    }

    nvimRpc::types::Dictionary nvim_get_commands(nvimRpc::types::Dictionary opts) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_commands", opts);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array nvim_get_api_info() {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("nvim_get_api_info");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_set_client_info(std::string name, nvimRpc::types::Dictionary version, std::string type,
                              nvimRpc::types::Dictionary methods, nvimRpc::types::Dictionary attributes) {
        auto packedResponse = _call<packer::Void>("nvim_set_client_info", name, version, type, methods, attributes);

        _handleResponse(packedResponse);
    }

    nvimRpc::types::Dictionary nvim_get_chan_info(int64_t chan) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_get_chan_info", chan);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array nvim_list_chans() {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("nvim_list_chans");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array nvim_call_atomic(nvimRpc::types::Array calls) {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("nvim_call_atomic", calls);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary nvim_parse_expression(std::string expr, std::string flags, bool highlight) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_parse_expression", expr, flags, highlight);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array nvim_list_uis() {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("nvim_list_uis");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array nvim_get_proc_children(int64_t pid) {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("nvim_get_proc_children", pid);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_get_proc(int64_t pid) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_get_proc", pid);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_select_popupmenu_item(int64_t item, bool insert, bool finish, nvimRpc::types::Dictionary opts) {
        auto packedResponse = _call<packer::Void>("nvim_select_popupmenu_item", item, insert, finish, opts);

        _handleResponse(packedResponse);
    }

    void nvim_set_decoration_provider(int64_t ns_id, nvimRpc::types::Dictionary opts) {
        auto packedResponse = _call<packer::Void>("nvim_set_decoration_provider", ns_id, opts);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_win_get_buf(msgpack::type::ext window) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_win_get_buf", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_buf(msgpack::type::ext window, msgpack::type::ext buffer) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_buf", window, buffer);

        _handleResponse(packedResponse);
    }

    std::vector<int64_t> nvim_win_get_cursor(msgpack::type::ext window) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("nvim_win_get_cursor", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_cursor(msgpack::type::ext window, std::vector<int64_t> pos) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_cursor", window, pos);

        _handleResponse(packedResponse);
    }

    int64_t nvim_win_get_height(msgpack::type::ext window) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_win_get_height", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_height(msgpack::type::ext window, int64_t height) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_height", window, height);

        _handleResponse(packedResponse);
    }

    int64_t nvim_win_get_width(msgpack::type::ext window) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_win_get_width", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_width(msgpack::type::ext window, int64_t width) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_width", window, width);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_win_get_var(msgpack::type::ext window, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_win_get_var", window, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_var(msgpack::type::ext window, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_var", window, name, value);

        _handleResponse(packedResponse);
    }

    void nvim_win_del_var(msgpack::type::ext window, std::string name) {
        auto packedResponse = _call<packer::Void>("nvim_win_del_var", window, name);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_win_get_option(msgpack::type::ext window, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_win_get_option", window, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_option(msgpack::type::ext window, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_option", window, name, value);

        _handleResponse(packedResponse);
    }

    std::vector<int64_t> nvim_win_get_position(msgpack::type::ext window) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("nvim_win_get_position", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext nvim_win_get_tabpage(msgpack::type::ext window) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_win_get_tabpage", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t nvim_win_get_number(msgpack::type::ext window) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("nvim_win_get_number", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool nvim_win_is_valid(msgpack::type::ext window) {
        bool ret;
        auto packedResponse = _call<bool>("nvim_win_is_valid", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_set_config(msgpack::type::ext window, nvimRpc::types::Dictionary config) {
        auto packedResponse = _call<packer::Void>("nvim_win_set_config", window, config);

        _handleResponse(packedResponse);
    }

    nvimRpc::types::Dictionary nvim_win_get_config(msgpack::type::ext window) {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("nvim_win_get_config", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void nvim_win_hide(msgpack::type::ext window) {
        auto packedResponse = _call<packer::Void>("nvim_win_hide", window);

        _handleResponse(packedResponse);
    }

    void nvim_win_close(msgpack::type::ext window, bool force) {
        auto packedResponse = _call<packer::Void>("nvim_win_close", window, force);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext nvim_win_call(msgpack::type::ext window, msgpack::type::ext fun) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("nvim_win_call", window, fun);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t buffer_line_count(msgpack::type::ext buffer) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("buffer_line_count", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<std::string> buffer_get_lines(msgpack::type::ext buffer, int64_t start, int64_t end,
                                              bool strict_indexing) {
        std::vector<std::string> ret;
        auto packedResponse = _call<std::vector<std::string>>("buffer_get_lines", buffer, start, end, strict_indexing);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void buffer_set_lines(msgpack::type::ext buffer, int64_t start, int64_t end, bool strict_indexing,
                          std::vector<std::string> replacement) {
        auto packedResponse = _call<packer::Void>("buffer_set_lines", buffer, start, end, strict_indexing, replacement);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext buffer_get_var(msgpack::type::ext buffer, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("buffer_get_var", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext buffer_get_option(msgpack::type::ext buffer, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("buffer_get_option", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void buffer_set_option(msgpack::type::ext buffer, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("buffer_set_option", buffer, name, value);

        _handleResponse(packedResponse);
    }

    std::string buffer_get_name(msgpack::type::ext buffer) {
        std::string ret;
        auto packedResponse = _call<std::string>("buffer_get_name", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void buffer_set_name(msgpack::type::ext buffer, std::string name) {
        auto packedResponse = _call<packer::Void>("buffer_set_name", buffer, name);

        _handleResponse(packedResponse);
    }

    bool buffer_is_valid(msgpack::type::ext buffer) {
        bool ret;
        auto packedResponse = _call<bool>("buffer_is_valid", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<int64_t> buffer_get_mark(msgpack::type::ext buffer, std::string name) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("buffer_get_mark", buffer, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t buffer_add_highlight(msgpack::type::ext buffer, int64_t ns_id, std::string hl_group, int64_t line,
                                 int64_t col_start, int64_t col_end) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("buffer_add_highlight", buffer, ns_id, hl_group, line, col_start, col_end);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::string vim_command_output(std::string command) {
        std::string ret;
        auto packedResponse = _call<std::string>("vim_command_output", command);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t buffer_get_number(msgpack::type::ext buffer) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("buffer_get_number", buffer);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void buffer_clear_highlight(msgpack::type::ext buffer, int64_t ns_id, int64_t line_start, int64_t line_end) {
        auto packedResponse = _call<packer::Void>("buffer_clear_highlight", buffer, ns_id, line_start, line_end);

        _handleResponse(packedResponse);
    }

    std::vector<msgpack::type::ext> tabpage_get_windows(msgpack::type::ext tabpage) {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("tabpage_get_windows", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext tabpage_get_var(msgpack::type::ext tabpage, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("tabpage_get_var", tabpage, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext tabpage_get_window(msgpack::type::ext tabpage) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("tabpage_get_window", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool tabpage_is_valid(msgpack::type::ext tabpage) {
        bool ret;
        auto packedResponse = _call<bool>("tabpage_is_valid", tabpage);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void ui_detach() {
        auto packedResponse = _call<packer::Void>("ui_detach");

        _handleResponse(packedResponse);
    }

    msgpack::type::ext ui_try_resize(int64_t width, int64_t height) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("ui_try_resize", width, height);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_command(std::string command) {
        auto packedResponse = _call<packer::Void>("vim_command", command);

        _handleResponse(packedResponse);
    }

    void vim_feedkeys(std::string keys, std::string mode, bool escape_csi) {
        auto packedResponse = _call<packer::Void>("vim_feedkeys", keys, mode, escape_csi);

        _handleResponse(packedResponse);
    }

    int64_t vim_input(std::string keys) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("vim_input", keys);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::string vim_replace_termcodes(std::string str, bool from_part, bool do_lt, bool special) {
        std::string ret;
        auto packedResponse = _call<std::string>("vim_replace_termcodes", str, from_part, do_lt, special);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_eval(std::string expr) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_eval", expr);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_call_function(std::string fn, nvimRpc::types::Array args) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_call_function", fn, args);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    int64_t vim_strwidth(std::string text) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("vim_strwidth", text);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<std::string> vim_list_runtime_paths() {
        std::vector<std::string> ret;
        auto packedResponse = _call<std::vector<std::string>>("vim_list_runtime_paths");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_change_directory(std::string dir) {
        auto packedResponse = _call<packer::Void>("vim_change_directory", dir);

        _handleResponse(packedResponse);
    }

    std::string vim_get_current_line() {
        std::string ret;
        auto packedResponse = _call<std::string>("vim_get_current_line");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_set_current_line(std::string line) {
        auto packedResponse = _call<packer::Void>("vim_set_current_line", line);

        _handleResponse(packedResponse);
    }

    void vim_del_current_line() {
        auto packedResponse = _call<packer::Void>("vim_del_current_line");

        _handleResponse(packedResponse);
    }

    msgpack::type::ext vim_get_var(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_get_var", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_get_vvar(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_get_vvar", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_get_option(std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_get_option", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_set_option(std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("vim_set_option", name, value);

        _handleResponse(packedResponse);
    }

    void vim_out_write(std::string str) {
        auto packedResponse = _call<packer::Void>("vim_out_write", str);

        _handleResponse(packedResponse);
    }

    void vim_err_write(std::string str) {
        auto packedResponse = _call<packer::Void>("vim_err_write", str);

        _handleResponse(packedResponse);
    }

    void vim_report_error(std::string str) {
        auto packedResponse = _call<packer::Void>("vim_report_error", str);

        _handleResponse(packedResponse);
    }

    std::vector<msgpack::type::ext> vim_get_buffers() {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("vim_get_buffers");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_get_current_buffer() {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_get_current_buffer");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_set_current_buffer(msgpack::type::ext buffer) {
        auto packedResponse = _call<packer::Void>("vim_set_current_buffer", buffer);

        _handleResponse(packedResponse);
    }

    std::vector<msgpack::type::ext> vim_get_windows() {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("vim_get_windows");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_get_current_window() {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_get_current_window");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_set_current_window(msgpack::type::ext window) {
        auto packedResponse = _call<packer::Void>("vim_set_current_window", window);

        _handleResponse(packedResponse);
    }

    std::vector<msgpack::type::ext> vim_get_tabpages() {
        std::vector<msgpack::type::ext> ret;
        auto packedResponse = _call<std::vector<msgpack::type::ext>>("vim_get_tabpages");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext vim_get_current_tabpage() {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("vim_get_current_tabpage");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void vim_set_current_tabpage(msgpack::type::ext tabpage) {
        auto packedResponse = _call<packer::Void>("vim_set_current_tabpage", tabpage);

        _handleResponse(packedResponse);
    }

    void vim_subscribe(std::string event) {
        auto packedResponse = _call<packer::Void>("vim_subscribe", event);

        _handleResponse(packedResponse);
    }

    void vim_unsubscribe(std::string event) {
        auto packedResponse = _call<packer::Void>("vim_unsubscribe", event);

        _handleResponse(packedResponse);
    }

    int64_t vim_name_to_color(std::string name) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("vim_name_to_color", name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Dictionary vim_get_color_map() {
        nvimRpc::types::Dictionary ret;
        auto packedResponse = _call<nvimRpc::types::Dictionary>("vim_get_color_map");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    nvimRpc::types::Array vim_get_api_info() {
        nvimRpc::types::Array ret;
        auto packedResponse = _call<nvimRpc::types::Array>("vim_get_api_info");

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext window_get_buffer(msgpack::type::ext window) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("window_get_buffer", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    std::vector<int64_t> window_get_cursor(msgpack::type::ext window) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("window_get_cursor", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void window_set_cursor(msgpack::type::ext window, std::vector<int64_t> pos) {
        auto packedResponse = _call<packer::Void>("window_set_cursor", window, pos);

        _handleResponse(packedResponse);
    }

    int64_t window_get_height(msgpack::type::ext window) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("window_get_height", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void window_set_height(msgpack::type::ext window, int64_t height) {
        auto packedResponse = _call<packer::Void>("window_set_height", window, height);

        _handleResponse(packedResponse);
    }

    int64_t window_get_width(msgpack::type::ext window) {
        int64_t ret;
        auto packedResponse = _call<int64_t>("window_get_width", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void window_set_width(msgpack::type::ext window, int64_t width) {
        auto packedResponse = _call<packer::Void>("window_set_width", window, width);

        _handleResponse(packedResponse);
    }

    msgpack::type::ext window_get_var(msgpack::type::ext window, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("window_get_var", window, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext window_get_option(msgpack::type::ext window, std::string name) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("window_get_option", window, name);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    void window_set_option(msgpack::type::ext window, std::string name, msgpack::type::ext value) {
        auto packedResponse = _call<packer::Void>("window_set_option", window, name, value);

        _handleResponse(packedResponse);
    }

    std::vector<int64_t> window_get_position(msgpack::type::ext window) {
        std::vector<int64_t> ret;
        auto packedResponse = _call<std::vector<int64_t>>("window_get_position", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    msgpack::type::ext window_get_tabpage(msgpack::type::ext window) {
        msgpack::type::ext ret;
        auto packedResponse = _call<msgpack::type::ext>("window_get_tabpage", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }

    bool window_is_valid(msgpack::type::ext window) {
        bool ret;
        auto packedResponse = _call<bool>("window_is_valid", window);

        _handleResponse(packedResponse, ret);

        return ret;
    }
};
} // namespace nvimRpc

#endif
