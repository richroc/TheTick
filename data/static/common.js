
function not_tick(){
    $.get("/mugga.txt", function (data) {
        if (data.trim() === "1") {
            $('.fa-spider').addClass('fa-horse-head').removeClass('fa-spider');
            $('.sidebar-brand-text')[0].innerHTML = "The Horse";
            document.title = document.title.replaceAll('Tick', 'Horse');
        }
    });
}

function get_editor_files_list(){
    $.get("/list?dir=/", function(data){
        data.forEach(f => {
            $(`<a class="collapse-item" href="/static/editor.html?file=${f.name}">${f.name}</a>`).insertAfter( "#config-menu" );
        });
    });
}


$(document).ready(function () {
    not_tick();
    get_editor_files_list();
});