#include <stdint.h>
#include "ContentDecryptionModule_10.h"
#include <Winsock2.h>
#include <Windows.h>




class  Host_10 {
public:
	static const int kVersion = 10;
    SOCKET sClient;

	ContentDecryptionModule_10 *mod;

	// Returns a Buffer* containing non-zero members upon success, or NULL on
	// failure. The caller owns the Buffer* after this call. The buffer is not
	// guaranteed to be zero initialized. The capacity of the allocated Buffer
	// is guaranteed to be not less than |capacity|.
	virtual Buffer* Allocate(uint32_t capacity)

	{
		Buffer* aa = NULL;

		return aa;


	}

	// Requests the host to call ContentDecryptionModule::TimerFired() |delay_ms|
	// from now with |context|.
	virtual void SetTimer(int64_t delay_ms, void* context)
	{


	}

	// Returns the current wall time.
	virtual Time GetCurrentWallTime() {

		Time aa = 0;

		return  aa;


	}
	
	// Called by the CDM with the result after the CDM instance was initialized.
	virtual void OnInitialized(bool success) {

		//printf("init %d\n", success);
	}

	// Called by the CDM when a key status is available in response to
	// GetStatusForPolicy().
	virtual void OnResolveKeyStatusPromise(uint32_t promise_id,
		KeyStatus key_status) {


	}

	// Called by the CDM when a session is created or loaded and the value for the
	// MediaKeySession's sessionId attribute is available (|session_id|).
	// This must be called before OnSessionMessage() or
	// OnSessionKeysChange() is called for the same session. |session_id_size|
	// should not include null termination.
	// When called in response to LoadSession(), the |session_id| must be the
	// same as the |session_id| passed in LoadSession(), or NULL if the
	// session could not be loaded.
	virtual void OnResolveNewSessionPromise(uint32_t promise_id,
		const char* session_id,
		uint32_t session_id_size) {

		//printf("OnResolveNewSessionPromise called: session_id is %s\n", session_id);


	}

	// Called by the CDM when a session is updated or released.
	virtual void OnResolvePromise(uint32_t promise_id) {

	//	printf("OnResolvePromise promise_id :%d\n", promise_id);


	}

	// Called by the CDM when an error occurs as a result of one of the
	// ContentDecryptionModule calls that accept a |promise_id|.
	// |exception| must be specified. |error_message| and |system_code|
	// are optional. |error_message_size| should not include null termination.
	virtual void OnRejectPromise(uint32_t promise_id,
		Exception exception,
		uint32_t system_code,
		const char* error_message,
		uint32_t error_message_size) {

		printf("OnRejectPromise  :%d ", promise_id);

		printf("error :%d\n", system_code);



	}
	// Called by the CDM when it has a message for session |session_id|.
	// Size parameters should not include null termination.
	virtual void OnSessionMessage(const char* session_id,
		uint32_t session_id_size,
		MessageType message_type,
		const char* message,
		uint32_t message_size) {

		printf("SessionMessage id  :0x%s\n", session_id);
		printf("Licence msg[%d] : \n", message_size);

		uint8_t* bytes = (uint8_t*)message;
		for (uint32_t i = 0; i < message_size; i++) {
			printf( "%02x", bytes[i]);
		}
		printf( "\n");
		send(sClient, message, message_size, 0);
		//dump_memory((void *)message, message_size);


	}

	// Called by the CDM when there has been a change in keys or their status for
	// session |session_id|. |has_additional_usable_key| should be set if a
	// key is newly usable (e.g. new key available, previously expired key has
	// been renewed, etc.) and the browser should attempt to resume playback.
	// |keys_info| is the list of key IDs for this session along with their
	// current status. |keys_info_count| is the number of entries in |keys_info|.
	// Size parameter for |session_id| should not include null termination.
	virtual void OnSessionKeysChange(const char* session_id,
		uint32_t session_id_size,
		bool has_additional_usable_key,
		const KeyInformation* keys_info,
		uint32_t keys_info_count) {

		printf("OnSessionKeysChange :\n");
		printf("KeyInformation keys_info[%d] : \n", keys_info->key_id_size);

		uint8_t* bytes = (uint8_t*)keys_info->key_id;
		for (uint32_t i = 0; i < keys_info->key_id_size; i++) {
			printf("%02x", bytes[i]);
		}
		printf("\n");


	}

	// Called by the CDM when there has been a change in the expiration time for
	// session |session_id|. This can happen as the result of an Update() call
	// or some other event. If this happens as a result of a call to Update(),
	// it must be called before resolving the Update() promise. |new_expiry_time|
	// represents the time after which the key(s) in the session will no longer
	// be usable for decryption. It can be 0 if no such time exists or if the
	// license explicitly never expires. Size parameter should not include null
	// termination.
	virtual void OnExpirationChange(const char* session_id,
		uint32_t session_id_size,
		Time new_expiry_time) {


	}

	// Called by the CDM when session |session_id| is closed. Size
	// parameter should not include null termination.
	virtual void OnSessionClosed(const char* session_id,
		uint32_t session_id_size) {


	}

	// The following are optional methods that may not be implemented on all
	// platforms.

	// Sends a platform challenge for the given |service_id|. |challenge| is at
	// most 256 bits of data to be signed. Once the challenge has been completed,
	// the host will call ContentDecryptionModule::OnPlatformChallengeResponse()
	// with the signed challenge response and platform certificate. Size
	// parameters should not include null termination.
	virtual void SendPlatformChallenge(const char* service_id,
		uint32_t service_id_size,
		const char* challenge,
		uint32_t challenge_size) {

		printf("SendPlatformChallenge call:\n");


	}

	// Attempts to enable output protection (e.g. HDCP) on the display link. The
	// |desired_protection_mask| is a bit mask of OutputProtectionMethods. No
	// status callback is issued, the CDM must call QueryOutputProtectionStatus()
	// periodically to ensure the desired protections are applied.
	virtual void EnableOutputProtection(uint32_t desired_protection_mask) {


	}

	// Requests the current output protection status. Once the host has the status
	// it will call ContentDecryptionModule::OnQueryOutputProtectionStatus().
	virtual void QueryOutputProtectionStatus() {


	}

	// Must be called by the CDM if it returned kDeferredInitialization during
	// InitializeAudioDecoder() or InitializeVideoDecoder().
	virtual void OnDeferredInitializationDone(StreamType stream_type,
		Status decoder_status) {


	}

	// Creates a FileIO object from the host to do file IO operation. Returns NULL
	// if a FileIO object cannot be obtained. Once a valid FileIO object is
	// returned, |client| must be valid until FileIO::Close() is called. The
	// CDM can call this method multiple times to operate on different files.
	virtual FileIO* CreateFileIO(FileIOClient* client) {


		FileIO *p = NULL;
		return p;


	}

	// Requests a specific version of the storage ID. A storage ID is a stable,
	// device specific ID used by the CDM to securely store persistent data. The
	// ID will be returned by the host via ContentDecryptionModule::OnStorageId().
	// If |version| is 0, the latest version will be returned. All |version|s
	// that are greater than or equal to 0x80000000 are reserved for the CDM and
	// should not be supported or returned by the host. The CDM must not expose
	// the ID outside the client device, even in encrypted form.
	virtual void RequestStorageId(uint32_t version) {


	}

	//protected:
	Host_10() {}
	virtual ~Host_10() {}
};
#pragma once

#pragma once
