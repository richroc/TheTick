function decodeWiegand(rawData, bitLength) {
  let binaryData = parseInt(rawData, 16).toString(2).padStart(bitLength, '0');
  let format, facilityCode, cardNumber;
  
  switch (bitLength) {
      case 26:
          format = "H10301";
          facilityCode = parseInt(binaryData.substring(1, 9), 2);
          cardNumber = parseInt(binaryData.substring(9, 25), 2);
          break;
      case 37:
          format = "H10304";
          facilityCode = parseInt(binaryData.substring(1, 17), 2);
          cardNumber = parseInt(binaryData.substring(17, 36), 2);
          break;
      case 34:
          format = "HID34";
          facilityCode = parseInt(binaryData.substring(2, 14), 2);
          cardNumber = parseInt(binaryData.substring(14, 32), 2);
          break;
      case 35:
          format = "HID35";
          facilityCode = parseInt(binaryData.substring(1, 21), 2);
          cardNumber = parseInt(binaryData.substring(21, 34), 2);
          break;
      case 50:
          format = "PACS50";
          facilityCode = parseInt(binaryData.substring(8, 24), 2);
          cardNumber = parseInt(binaryData.substring(24, 48), 2);
          break;
      default:
          format = "Unknown";
          facilityCode = "N/A";
          cardNumber = "N/A";
  }
  
  return { format, facilityCode, cardNumber };
}

function calculateWiegandParity(rawData, bitLength) {
    let binaryData = parseInt(rawData, 16).toString(2).padStart(bitLength - 2, '0');
    let evenParity, oddParity;
  
    switch (bitLength) {
      case 26:
        evenParity = binaryData.substring(0, 12).split('').filter(bit => bit === '1').length % 2 === 0 ? 0 : 1;
        oddParity = binaryData.substring(12).split('').filter(bit => bit === '1').length % 2 === 1 ? 0 : 1;
        break;
      case 37:
        evenParity = binaryData.substring(0, 18).split('').filter(bit => bit === '1').length % 2 === 0 ? 0 : 1;
        oddParity = binaryData.substring(18).split('').filter(bit => bit === '1').length % 2 === 1 ? 0 : 1;
        break;
      case 34:
        evenParity = binaryData.substring(0, 16).split('').filter(bit => bit === '1').length % 2 === 0 ? 0 : 1;
        oddParity = binaryData.substring(16).split('').filter(bit => bit === '1').length % 2 === 1 ? 0 : 1;
        break;
      case 35:
        evenParity = binaryData.substring(0, 17).split('').filter(bit => bit === '1').length % 2 === 0 ? 0 : 1;
        oddParity = binaryData.substring(17).split('').filter(bit => bit === '1').length % 2 === 1 ? 0 : 1;
        break;
      case 50:
        evenParity = binaryData.substring(0, 24).split('').filter(bit => bit === '1').length % 2 === 0 ? 0 : 1;
        oddParity = binaryData.substring(24).split('').filter(bit => bit === '1').length % 2 === 1 ? 0 : 1;
        break;
      default:
        return { error: "Unsupported bit length" };
    }
  
return { evenParity, oddParity };
}

function encodeWiegand(format, facilityCode, cardNumber) {
    let bitLength;
    let binaryData;

    switch (format) {
        case "H10301":
        bitLength = 26;
        binaryData = facilityCode.toString(2).padStart(8, '0') + cardNumber.toString(2).padStart(16, '0');
        break;
        case "H10304":
        bitLength = 37;
        binaryData = facilityCode.toString(2).padStart(16, '0') + cardNumber.toString(2).padStart(19, '0');
        break;
        case "HID34":
        bitLength = 34;
        binaryData = facilityCode.toString(2).padStart(12, '0') + cardNumber.toString(2).padStart(18, '0');
        break;
        case "HID35":
        bitLength = 35;
        binaryData = facilityCode.toString(2).padStart(20, '0') + cardNumber.toString(2).padStart(13, '0');
        break;
        case "PACS50":
        bitLength = 50;
        binaryData = facilityCode.toString(2).padStart(16, '0') + cardNumber.toString(2).padStart(24, '0');
        break;
        default:
        return { error: "Unsupported format" };
    }

    let { evenParity, oddParity } = calculateWiegandParity(parseInt(binaryData, 2).toString(16), bitLength);
    let encodedData = evenParity.toString() + binaryData + oddParity.toString();
    return parseInt(encodedData, 2).toString(16).toUpperCase() + ":" + bitLength.toString(10);
}
  

function send_wiegand(data) {
  $.get("/txid?v=" + data);
  alert(`Data sent: ${data}`);
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
  $.get("/log.txt", function (data) {
      let lines = data.trim().split("\n");
      let tableBody = "";
      
      lines.forEach(line => {
          let parts = line.split(" ");
          if (parts.length === 2) {
              let wiegand = parts[1].trim()
              let dataParts = wiegand.split(":");
              if (dataParts.length === 2) {
                  let rawData = dataParts[0];
                  let bitLength = parseInt(dataParts[1], 10);

                  if(rawData.length != bitLength){
                    let { format, facilityCode, cardNumber } = decodeWiegand(rawData, bitLength);
                    tableBody += `<tr><td>${parts[0]}</td><td>${wiegand}</td><td>${format}</td><td>${facilityCode}</td><td>${cardNumber}</td><td><i class="fas fa-reply" onclick="send_wiegand('${wiegand}');"></i></td></tr>`;
                  }
              }
          }
      });
      $("#dataTable tbody").html(tableBody);
      $('#dataTable').DataTable();
  });
});
