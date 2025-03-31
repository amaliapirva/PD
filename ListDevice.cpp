#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <RegStr.h>
#include <iostream>
#include <vector>
#include <usbioctl.h>

#pragma comment(lib, "Setupapi.lib")


void ListUSBDevicesCV() {
	HANDLE hHubDevice;
	char deviceName[256];
	for (int i = 0; i < 10; i++) {
		sprintf_s(deviceName, "\\\\.\\HCD%d", i);
		hHubDevice = CreateFileA(deviceName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (hHubDevice == INVALID_HANDLE_VALUE) {
			continue;
		}
		USB_NODE_INFORMATION hubInfo;
		DWORD bytesReturned;
		if (DeviceIoControl(hHubDevice, IOCTL_USB_GET_NODE_INFORMATION, NULL, 0, &hubInfo, sizeof(hubInfo), &bytesReturned, NULL))
		{
			int portCount = hubInfo.u.HubInformation.HubDescriptor.bNumberOfPorts;
			std::cout << "HUB has " << portCount << " ports\n";
			for (int port = 1; port <= portCount; port++)
			{
				USB_NODE_CONNECTION_INFORMATION_EX connectionInfo = { 0 };
				connectionInfo.ConnectionIndex = port;
				if (DeviceIoControl(hHubDevice, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION_EX, &connectionInfo, sizeof(connectionInfo), &connectionInfo, sizeof(connectionInfo), &bytesReturned, NULL))
				{
					if (connectionInfo.ConnectionStatus == DeviceConnected)
					{
						std::cout << " Port " << port << ": USB Device Connected\n";
						USB_DESCRIPTOR_REQUEST descriptorRequest = { 0 };
						descriptorRequest.ConnectionIndex = port;
						descriptorRequest.SetupPacket.wValue = USB_STRING_DESCRIPTOR_TYPE << 8;
						descriptorRequest.SetupPacket.wLength = sizeof(unsigned char);
						if (DeviceIoControl(hHubDevice, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION, &descriptorRequest, sizeof(descriptorRequest), &descriptorRequest, sizeof(descriptorRequest), &bytesReturned, NULL))
						{
							std::cout << "   Device Name : " << reinterpret_cast<char*>(descriptorRequest.Data) << std::endl;
						}
					}
				}
			}
		}
		CloseHandle(hHubDevice);
	}
}

int main() {
	std::cout << "Listing all connected USB devices:\n";
	ListUSBDevicesCV();
	return 0;
}
