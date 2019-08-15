#include "sdkconfig.h"
#include <esp_log.h>
#include <string>
#include <sstream>
#include <sys/time.h>
#include "BLEDevice.h"

#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include "Task.h"

#include "adc_read.h"
#include "pin_read.h"


static char LOG_TAG[] = "BLE MIDI Test";
extern xQueueHandle trig_queue;
uint8_t note = 64;

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"


uint8_t midiPacket[] = {
   0x80,  // header
   0x80,  // timestamp, not implemented 
   0x00,  // status
   0x3c,  // 0x3c == 60 == middle c
   0x00   // velocity
};

  // PotConditioner(Adc& adc) : adc_(adc) {}

class SendMIDITask: public Task {
public:
	xQueueHandle *triggerQueue_;

	SendMIDITask(xQueueHandle triggerQueue) { 
		triggerQueue_ = &triggerQueue;
	}

	void run(void *data) {
		// uint16_t adc_val = 0;
    	gpio_num_t io_num;
		BaseType_t xTaskWokenByReceive = pdFALSE;

		ESP_LOGI("SendMIDITask", "Running...\n");

		while(1) {
			if (xQueueReceive(trig_queue, &io_num, portMAX_DELAY)) {
				note = (note + 17) & 0x7F;
				// note down
				midiPacket[2] = 0x90; // note down, channel 0
				midiPacket[3] = note;
				midiPacket[4] = 127;  // velocity
				ESP_LOGI(LOG_TAG, "Note Down = %d", note);

				pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
				pCharacteristic->notify();
				// play note for 500ms
				delay(100);

				// note up
				midiPacket[2] = 0x80; // note up, channel 0
				midiPacket[3] = note;
				midiPacket[4] = 0;    // velocity
				ESP_LOGI(LOG_TAG, "Note Up = %d", note);

				pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
				pCharacteristic->notify();
			}
		}
	} // run

}; // SendMIDITask

SendMIDITask *pSendMIDITask;

class MIDIServerCallbacks: public BLEServerCallbacks {
	void onConnect(BLEServer* pServer) {
		pSendMIDITask->start();
	};

	void onDisconnect(BLEServer* pServer) {
		pSendMIDITask->stop();
		// pSendMIDITask->noteoff();
	}
};

void start_ble_midi_server(xQueueHandle *queue)
{
	uint32_t properites = BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE_NR;

	ESP_LOGI("start_ble_midi_server()"," got &queue addr = 0x%08x\n", (uint32_t)(&queue));
	ESP_LOGI("start_ble_midi_server()"," got queue addr = 0x%08x\n", (uint32_t)(queue));

	pSendMIDITask = new SendMIDITask(queue);
	pSendMIDITask->setStackSize(8000);

	// Create the BLE Device
	BLEDevice::init("Test BLE MIDI");

	// Create the BLE Server
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MIDIServerCallbacks());
  	BLEDevice::setEncryptionLevel((esp_ble_sec_act_t)ESP_LE_AUTH_REQ_SC_BOND);

	// Create the BLE Service
	BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));
	ESP_LOGI(LOG_TAG, "Created Service %s", MIDI_SERVICE_UUID);

	// Create a BLE Characteristic
	pCharacteristic = pService->createCharacteristic(BLEUUID(MIDI_CHARACTERISTIC_UUID), properites);
 	pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
	pCharacteristic->addDescriptor(new BLE2902());

	// Start the service
	pService->start();

	// Start advertising
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

	pServer->getAdvertising()->setMinInterval(0x20);
	pServer->getAdvertising()->setMaxInterval(0x40);
	pServer->getAdvertising()->addServiceUUID(MIDI_SERVICE_UUID);
	pServer->getAdvertising()->start();
}
