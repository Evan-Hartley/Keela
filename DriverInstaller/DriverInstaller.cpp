//
// Created by ehart on 10/26/2026.
//

// Global cpp includes, could move to a precompiled header
#include "DriverInstaller.h"

// Function which checks to see if the USB device detected has libusbK installed
bool IsLibUsbKInstalled()
{
	// Fetch the information from detected USB device
    HDEVINFO deviceInfo =
        SetupDiGetClassDevs(
            nullptr,
            "USB",
            nullptr,
            DIGCF_PRESENT | DIGCF_ALLCLASSES);

	// If not an appropriate device, return false
    if (deviceInfo == INVALID_HANDLE_VALUE)
        return false;

	// Store the data of the device
    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD index = 0;

	// Assume libusbK isn't installed unless it is detected
    bool foundLibUsbK = false;
	// Sort through the device info to see if libusbK can be found, if so change bool to true
    while (SetupDiEnumDeviceInfo(
        deviceInfo,
        index++,
        &devInfoData))
    {
        char buffer[1024];
        DWORD dataType = 0;
        if (SetupDiGetDeviceRegistryPropertyA(
                deviceInfo,
                &devInfoData,
                SPDRP_SERVICE,
                &dataType,
                reinterpret_cast<PBYTE>(buffer),
                sizeof(buffer),
                nullptr))
        {
            std::string service(buffer);
            if (service == "libusbk")
            {
                foundLibUsbK = true;
                break;
            }
        }
    }

	// For the device, return the result
    SetupDiDestroyDeviceInfoList(deviceInfo);
    return foundLibUsbK;
}

// Install libusbK to a GENICam Camera
bool InstallLibUsbKForDevice(
    uint16_t vid,
    uint16_t pid)
{
    struct wdi_device_info* list = nullptr;
    if (wdi_create_list(&list, nullptr)
        != WDI_SUCCESS)
    {
        return false;
    }
    for (auto dev = list;
         dev != nullptr;
         dev = dev->next)
    {
        if (dev->vid == vid &&
            dev->pid == pid)
        {
            wdi_options_prepare_driver prep = {};
            wdi_options_install_driver install = {};
            int result =
                wdi_prepare_driver(
                    dev,
                    "driver",
                    "GenICam Camera",
                    "libusbK",
                    &prep);
            if (result != WDI_SUCCESS)
            {
                wdi_destroy_list(list);
                return false;
            }
            result =
                wdi_install_driver(
                    dev,
                    "driver",
                    "GenICam Camera.inf",
                    &install);
            wdi_destroy_list(list);
            return result == WDI_SUCCESS;
        }
    }
    wdi_destroy_list(list);
    return false;
}

int main()
{
	// Use arv to detect GenICam cameras
    arv_update_device_list();
    guint nDevices =
        arv_get_n_devices();

	// Inform user how many cameras were found
    if (nDevices == 0)
    {
        spdlog::warn("No GenICam cameras found.");
        return 0;
    }
    spdlog::info("Found ", nDevices, " GenICam camera(s).");

	// Sort through cameras and insure libusbK is installed, if installation is preformed inform user
    for (guint i = 0; i < nDevices; ++i)
    {
        const char* id =
            arv_get_device_id(i);
            spdlog::info("Camera: ", id);
        
        // Open camera
        ArvDevice* device =
            arv_open_device(id, nullptr);
        if (!device)
            continue;
        
        // Get USB3 Vision information from the device
        uint16_t vid = arv_get_device_vendor(device);
        uint16_t pid = arv_get_device_model(device);

        if (!IsLibUsbKInstalled())
        {
            spdlog::info( "Installing libusbK...");
            InstallLibUsbKForDevice(
                vid,
                pid);
        }
		else
		{
			spdlog::info( "Already running on libusbK.");
		}
        g_object_unref(device);
    }
    return 0;
}