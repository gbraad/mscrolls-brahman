Module.preRun.push(function() {

    window.open_url = function(url, url_len) {
        const url_string = UTF8ToString(url, url_len);
        window.open(url_string, '_blank');
    }
});

