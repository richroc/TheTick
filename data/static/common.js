
function not_tick() {
    $.get("/mugga.txt", function (data) {
        if (data.trim() === "1") {
            $('.fa-spider').addClass('fa-horse-head').removeClass('fa-spider');
            $('.sidebar-brand-text')[0].innerHTML = "The Horse";
            document.title = document.title.replaceAll('Tick', 'Horse');
        }
    });
}

function get_editor_files_list() {
    $.get("/list?dir=/", function (data) {
        data.forEach(f => {
            $(`<a class="collapse-item" href="/editor.html?file=${f.name}">${f.name}</a>`).insertAfter("#config-menu");
        });
    });
}


function remove_menu_option(name) {
    $('#menu_' + name).remove();
}

function get_version_info() {
    return $.getJSON("/version?epoch=" + Date.now(), function (data) {
        let configuration = data;

        if (configuration['features'].indexOf('wiegand') == -1)
            remove_menu_option('mode_wiegand');

        if (configuration['features'].indexOf('clockanddata') == -1)
            remove_menu_option('mode_clockanddata');

        if (configuration['features'].indexOf('ota_http') == -1)
            remove_menu_option('update');

        if (configuration['mode'] == 'wiegand')
            $('#menu_current_mode')[0].innerHTML = "Wiegand";
        if (configuration['mode'] == 'clockanddata')
            $('#menu_current_mode')[0].innerHTML = "Clock&amp;Data";
        if (configuration['mode'] == 'osdp')
            $('#menu_current_mode')[0].innerHTML = "OSDP";

        $('#menu_version')[0].innerHTML = configuration['version'];

        $('#menu_board_name')[0].innerHTML = configuration['log_name'] + '-' + configuration['ChipID'];


        console.log(data);
    });
}

function get_epochs(lines) {
    let epochs = {};
    lines.forEach(line => {
        let parts = line.split("; ");
        if (parts[2] == "epoch") {
            epochs[parts[0]] = parseInt(parts[3]) - parseInt(parts[1]);
        }
    });
    return epochs;
}

function get_epoch_time(epochs, epoch, timestamp) {
    if (epoch in epochs) {
        let date = new Date(epochs[epoch] + parseInt(timestamp))
        return date.toLocaleString()
    } else {
        return "BC: " + epoch + " TS: " + timestamp;
    }
}

$(document).ready(function () {
    get_version_info();
    not_tick();
    get_editor_files_list();
});