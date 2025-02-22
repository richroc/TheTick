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
      case 46:
          format = "H800002";
          facilityCode = parseInt(binaryData.substring(1, 15), 2);
          cardNumber = parseInt(binaryData.substring(15, 45), 2);
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
      case 46:
        evenParity = binaryData.split('').filter(bit => bit === '1').length % 2 === 0 ? 0 : 1;
        oddParity = binaryData.split('').filter(bit => bit === '1').length % 2 === 1 ? 0 : 1;
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
        case "H800002":
        bitLength = 46;
        binaryData = facilityCode.toString(2).padStart(14, '0') + cardNumber.toString(2).padStart(30, '0');
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
