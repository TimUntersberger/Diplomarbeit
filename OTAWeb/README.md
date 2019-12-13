# OTAWeb

## Things that are working right now

* Adding a firmware to the list (Press the add firmware button)
* Updating the firmware (The arrow up icon)
* Downloading the firmware ([See example below](#working-example))
* Basic datatable sorting based on column (Click on a table header)

## Things that are missing

* Downloading the firmware based on the name given add creation time
* Changing the name given to the firmware (Should we allow this?)

## Downloading a firmware in production

```bash
curl https://ota.baaka.io/api/firmware/<id>/download
```

### Working example
Downloads OTA
```bash
curl https://ota.baaka.io/api/firmware/0/download
```
