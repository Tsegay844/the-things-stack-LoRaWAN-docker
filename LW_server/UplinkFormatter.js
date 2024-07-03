function decodeUplink(input) {
  let bytes = input.bytes;
  // bytes 0-2 App Dev ID
  let appDevAddr = input.bytes.slice(0, 3); // slice first three bytes - Application Device identification (CxFFEE), where x is the sensor number
  let appDevAddrString = appDevAddr.map(byte => byte.toString(16).padStart(2, '0')).join('');
  // bytes 3 Duty Cycle Interval (minutes)
  let dcInterval = bytes[3];
  // bytes 4 and 5 Frame Counter
  let counterValue = (bytes[4] << 8) | bytes[5];
  // bytes 6 and 7 SHT30 temperature Celsius
  let sht30Temp = bytes[6] +(bytes[7]/100);
  // bytes 8 and 9 SHT30 relative humidity %
  let sht30Hum = bytes[8] +(bytes[9]/100);
  // bytes 10 and 11 BMP280 temperature Celsius
  let bmp280Temp = bytes[10] +(bytes[11]/100);
  // bytes 12-15 BMP280 pressure Pascal
  let bmp280Pres = (bytes[12] << 24) | (bytes[13] << 16) | (bytes[14] << 8) | bytes[15];
  // bytes 16 Battery level (254=full)
  let batteryLv = (bytes[16] / 254 * 100).toFixed(2);

  return {
    data: {
      appDevId: appDevAddrString,
      dcInt: dcInterval,
      pktCtr: counterValue,
      shtT: sht30Temp,
      shtRH: sht30Hum,
      bmpT: bmp280Temp,
      bmpP: bmp280Pres,
      batLv: batteryLv
    },
    warnings: [],
    errors: []
  };
}

// Example usage
let input = { bytes: [0xC0,0xFF,0xEE,0x1E,0x01,0x22,0x1F,0x41,0x2D,0x2A,0x21,0x08,0x00,0x01,0x89,0xFF,0xFE] };
let result = decodeUplink(input);
console.log(result);
