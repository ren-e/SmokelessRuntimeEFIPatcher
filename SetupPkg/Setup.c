/* Start native AMI BIOS Setup */
#include <Uefi.h>
#include <Protocol/FormBrowser2.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define EFI_SETUPMODE_FORMID 0x2710

STATIC EFI_GUID ami_setup_guid = {
	0x899407d7, 0x99fe, 0x43d8, { 0x9a, 0x21, 0x79, 0xec, 0x32, 0x8c, 0xac, 0x21 }
};


EFI_STATUS EFIAPI setup(EFI_HANDLE image, EFI_SYSTEM_TABLE *st)
{
	EFI_STATUS status;
	EFI_FORM_BROWSER2_PROTOCOL *fb;
	EFI_HII_HANDLE *handles;
	EFI_BROWSER_ACTION_REQUEST action = EFI_BROWSER_ACTION_REQUEST_NONE;
	UINTN i;

	status = gBS->LocateProtocol(&gEfiFormBrowser2ProtocolGuid, NULL,
	    (VOID **)&fb);
	if (EFI_ERROR(status))
		return (status);

	handles = HiiGetHiiHandles(&ami_setup_guid);
	if (handles == NULL)
		return (EFI_NOT_FOUND);

	if (handles[0] == NULL) {
		status = EFI_NOT_FOUND;
		goto done;
	}

	for (i = 1; handles[i] != NULL; i++)
		;

	status = fb->SendForm(fb, handles, i, &gEfiHiiPlatformSetupFormsetGuid,
	    EFI_SETUPMODE_FORMID, NULL, &action);

done:
	gBS->FreePool(handles);

	if (action == EFI_BROWSER_ACTION_REQUEST_RESET)
		gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);

	return (status);
}
