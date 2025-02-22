// Copyright (C) 2024, 2025 Jakub "lenwe" Kramarz
// This file is part of The Tick.
// 
// The Tick is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// The Tick is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License

function send_wiegand(data) {
  $.get("/txid?v=" + data);
}


function send_wiegand_raw(event){
    event.preventDefault();
    let wiegand_raw = $("#wiegand_raw")[0].value;
    send_wiegand(wiegand_raw);

    return false;
}

function send_wiegand_formatted(event){
    event.preventDefault();
    let wiegand_format = $("#wiegand_format")[0].value;
    let wiegand_fc = parseInt($("#wiegand_fc")[0].value, 10);
    let wiegand_cn = parseInt($("#wiegand_cn")[0].value, 10);

    let wiegand_raw = encodeWiegand(
        wiegand_format,
        wiegand_fc,
        wiegand_cn
    );

    send_wiegand(wiegand_raw);

    return false;
}


$(document).ready(function () {
  var urlParams = new URLSearchParams(window.location.search);
  var filename = urlParams.get('file');

  $('#filename')[0].innerHTML = filename;

  var editor = CodeMirror.fromTextArea(document.getElementById("fileContent"), {
    lineNumbers: true,
    mode: "plaintext"
  });

  $.get(`/${filename}`, function(data) {
    editor.setValue(data);
  }).fail(function() {
      alert('Error loading file.');
  });

  $('#saveFile').click(function() {
    var content = editor.getValue();

    var formData = new FormData();
    var blob = new Blob([content], { type: 'text/plain' });
    formData.append("file", blob, filename);
    
    $.ajax({
        url: '/edit',
        type: 'POST',
        data: formData,
        processData: false,
        contentType: false,
        success: function() {
            alert('File saved successfully.');
        },
        error: function() {
            alert('Error saving file.');
        }
    });
  });

});
