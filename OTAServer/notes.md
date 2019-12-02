https://stackoverflow.com/questions/21397809/create-a-trusted-self-signed-ssl-cert-for-localhost-for-use-with-express-node

how to create self signed ssl certificate (decided to not use it because we would have to track the ip of the https server everytime we connect to a new wifi)

https://www.digitalocean.com/community/tutorials/how-to-secure-nginx-with-let-s-encrypt-on-ubuntu-16-04

how to secure nginx with letsencrypt (on private server)

Updated EspWifiManager, because it wasn't implemented. We thought it was already working but it was just a dummy implementation.

Needed to take root crt for it to work
https://github.com/espressif/esp-mqtt/issues/84

espidf tools missing //didnt work
https://docs.espressif.com/projects/esp-idf/en/latest/get-started/

platformio how to create new partiotion-table(needed for ota)
https://docs.espressif.com/projects/esp-jumpstart/en/latest/firmwareupgrade.html
https://docs.platformio.org/en/latest/platforms/espressif32.html#partition-tables

how to get certificate into file (was in code before)
http://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data
