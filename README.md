esp32_remote_ota_version.ino

This can be used to connect to a remote service and refresh the firmware.

The firmware must include the code to run the refresh regularly

Steps to run this code:
1. Copy the file to Arduino and update ssid, password and version_url.
2. Create a file version-url.txt with the below 2 lines:
- 1.0.0
- http://192.168.0.102:8080/firmare.bin
3. Start the server. Eg python python -m http.server 8080
4. Upload the image to the device
5. Check the serial port that the wifi is connected and it says nothing to update.
6. Now, update version to 1.0.1 in the version-url.txt.
7. Check that the new firmware is uploaded to the device.

To make it repeatable, ensure that the code in esp32_remote_ota_version.ino is there in firmware.bin.


