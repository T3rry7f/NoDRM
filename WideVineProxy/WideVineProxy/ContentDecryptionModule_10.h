#pragma once
#include <stdint.h>

struct Size {
	int32_t width;
	int32_t height;
};
enum Status : uint32_t {
	kSuccess = 0,
	kNeedMoreData,  // Decoder needs more data to produce a decoded frame/sample.
	kNoKey,         // The required decryption key is not available.
	kInitializationError,    // Initialization error.
	kDecryptError,           // Decryption failed.
	kDecodeError,            // Error decoding audio or video.
	kDeferredInitialization  // Decoder is not ready for initialization.
};

// Exceptions used by the CDM to reject promises.
// https://w3c.github.io/encrypted-media/#exceptions
enum Exception : uint32_t {
	kExceptionTypeError,
	kExceptionNotSupportedError,
	kExceptionInvalidStateError,
	kExceptionQuotaExceededError
};

// The encryption scheme. The definitions are from ISO/IEC 23001-7:2016.
enum class EncryptionScheme : uint32_t {
	kUnencrypted = 0,
	kCenc,  // 'cenc' subsample encryption using AES-CTR mode.
	kCbcs   // 'cbcs' pattern encryption using AES-CBC mode.
};

// The pattern used for pattern encryption. Note that ISO/IEC 23001-7:2016
// defines each block to be 16-bytes.
struct Pattern {
	uint32_t crypt_byte_block;  // Count of the encrypted blocks.
	uint32_t skip_byte_block;   // Count of the unencrypted blocks.
};

enum class ColorRange : uint8_t {
	kInvalid,
	kLimited,  // 709 color range with RGB values ranging from 16 to 235.
	kFull,     // Full RGB color range with RGB values from 0 to 255.
	kDerived   // Range is defined by |transfer_id| and |matrix_id|.
};

// Described in ISO 23001-8:2016, section 7. All the IDs are in the range
// [0, 255] so 8-bit integer is sufficient. An unspecified ColorSpace should be
// {2, 2, 2, ColorRange::kInvalid}, where value 2 means "Unspecified" for all
// the IDs, as defined by the spec.
struct ColorSpace {
	uint8_t primary_id;   // 7.1 colour primaries, table 2
	uint8_t transfer_id;  // 7.2 transfer characteristics, table 3
	uint8_t matrix_id;    // 7.3 matrix coefficients, table 4
	ColorRange range;
};

// Time is defined as the number of seconds since the Epoch
// (00:00:00 UTC, January 1, 1970), not including any added leap second.
// Also see Time definition in spec: https://w3c.github.io/encrypted-media/#time
// Note that Time is defined in millisecond accuracy in the spec but in second
// accuracy here.
typedef double Time;

// An input buffer can be split into several continuous subsamples.
// A SubsampleEntry specifies the number of clear and cipher bytes in each
// subsample. For example, the following buffer has three subsamples:
//
// |<----- subsample1 ----->|<----- subsample2 ----->|<----- subsample3 ----->|
// |   clear1   |  cipher1  |  clear2  |   cipher2   | clear3 |    cipher3    |
//
// For decryption, all of the cipher bytes in a buffer should be concatenated
// (in the subsample order) into a single logical stream. The clear bytes should
// not be considered as part of decryption.
//
// Stream to decrypt:   |  cipher1  |   cipher2   |    cipher3    |
// Decrypted stream:    | decrypted1|  decrypted2 |   decrypted3  |
//
// After decryption, the decrypted bytes should be copied over the position
// of the corresponding cipher bytes in the original buffer to form the output
// buffer. Following the above example, the decrypted buffer should be:
//
// |<----- subsample1 ----->|<----- subsample2 ----->|<----- subsample3 ----->|
// |   clear1   | decrypted1|  clear2  |  decrypted2 | clear3 |   decrypted3  |
//
struct SubsampleEntry {
	uint32_t clear_bytes;
	uint32_t cipher_bytes;
};

// Represents an input buffer to be decrypted (and possibly decoded). It does
// not own any pointers in this struct. If |encryption_scheme| = kUnencrypted,
// the data is unencrypted.
// Note that this struct is organized so that sizeof(InputBuffer_2)
// equals the sum of sizeof() all members in both 32-bit and 64-bit compiles.
// Padding has been added to keep the fields aligned.
struct InputBuffer_2 {
	const uint8_t* data;  // Pointer to the beginning of the input data.
	uint32_t data_size;   // Size (in bytes) of |data|.

	EncryptionScheme encryption_scheme;

	const uint8_t* key_id;  // Key ID to identify the decryption key.
	uint32_t key_id_size;   // Size (in bytes) of |key_id|.
	uint32_t : 32;          // Padding.

			   const uint8_t* iv;  // Initialization vector.
			   uint32_t iv_size;   // Size (in bytes) of |iv|.
			   uint32_t : 32;      // Padding.

						  const struct SubsampleEntry* subsamples;
						  uint32_t num_subsamples;  // Number of subsamples in |subsamples|.
						  uint32_t : 32;            // Padding.

													// |pattern| is required if |encryption_scheme| specifies pattern encryption.
									 Pattern pattern;

									 int64_t timestamp;  // Presentation timestamp in microseconds.
};

enum AudioCodec : uint32_t { kUnknownAudioCodec = 0, kCodecVorbis, kCodecAac };

struct AudioDecoderConfig_2 {
	AudioCodec codec;
	int32_t channel_count;
	int32_t bits_per_channel;
	int32_t samples_per_second;

	// Optional byte data required to initialize audio decoders, such as the
	// vorbis setup header.
	uint8_t* extra_data;
	uint32_t extra_data_size;

	// Encryption scheme.
	EncryptionScheme encryption_scheme;
};


// Supported sample formats for AudioFrames.
enum AudioFormat : uint32_t {
	kUnknownAudioFormat = 0,  // Unknown format value. Used for error reporting.
	kAudioFormatU8,           // Interleaved unsigned 8-bit w/ bias of 128.
	kAudioFormatS16,          // Interleaved signed 16-bit.
	kAudioFormatS32,          // Interleaved signed 32-bit.
	kAudioFormatF32,          // Interleaved float 32-bit.
	kAudioFormatPlanarS16,    // Signed 16-bit planar.
	kAudioFormatPlanarF32,    // Float 32-bit planar.
};


// Surface formats based on FOURCC labels, see: http://www.fourcc.org/yuv.php
// Values are chosen to be consistent with Chromium's VideoPixelFormat values.
enum VideoFormat : uint32_t {
	kUnknownVideoFormat = 0,  // Unknown format value. Used for error reporting.
	kYv12 = 1,                // 12bpp YVU planar 1x1 Y, 2x2 VU samples.
	kI420 = 2,                // 12bpp YUV planar 1x1 Y, 2x2 UV samples.

							  // In the following formats, each sample uses 16-bit in storage, while the
							  // sample value is stored in the least significant N bits where N is
							  // specified by the number after "P". For example, for YUV420P9, each Y, U,
							  // and V sample is stored in the least significant 9 bits in a 2-byte block.
							  kYUV420P9 = 16,
							  kYUV420P10 = 17,
							  kYUV422P9 = 18,
							  kYUV422P10 = 19,
							  kYUV444P9 = 20,
							  kYUV444P10 = 21,
							  kYUV420P12 = 22,
							  kYUV422P12 = 23,
							  kYUV444P12 = 24,
};



enum VideoCodec : uint32_t {
	kUnknownVideoCodec = 0,
	kCodecVp8,
	kCodecH264,
	kCodecVp9,
	kCodecAv1
};

enum VideoCodecProfile : uint32_t {
	kUnknownVideoCodecProfile = 0,
	kProfileNotNeeded,
	kH264ProfileBaseline,
	kH264ProfileMain,
	kH264ProfileExtended,
	kH264ProfileHigh,
	kH264ProfileHigh10,
	kH264ProfileHigh422,
	kH264ProfileHigh444Predictive,
	kVP9Profile0,
	kVP9Profile1,
	kVP9Profile2,
	kVP9Profile3,
	kAv1ProfileMain,
	kAv1ProfileHigh,
	kAv1ProfilePro
};

// Deprecated: New CDM implementations should use VideoDecoderConfig_3.
// Note that this struct is organized so that sizeof(VideoDecoderConfig_2)
// equals the sum of sizeof() all members in both 32-bit and 64-bit compiles.
// Padding has been added to keep the fields aligned.
struct VideoDecoderConfig_2 {
	VideoCodec codec;
	VideoCodecProfile profile;
	VideoFormat format;
	uint32_t : 32;  // Padding.

					// Width and height of video frame immediately post-decode. Not all pixels
					// in this region are valid.
			   Size coded_size;

			   // Optional byte data required to initialize video decoders, such as H.264
			   // AAVC data.
			   uint8_t* extra_data;
			   uint32_t extra_data_size;

			   // Encryption scheme.
			   EncryptionScheme encryption_scheme;
};

struct VideoDecoderConfig_3 {
	VideoCodec codec;
	VideoCodecProfile profile;
	VideoFormat format;
	ColorSpace color_space;

	// Width and height of video frame immediately post-decode. Not all pixels
	// in this region are valid.
	Size coded_size;

	// Optional byte data required to initialize video decoders, such as H.264
	// AAVC data.
	uint8_t* extra_data;
	uint32_t extra_data_size;

	EncryptionScheme encryption_scheme;
};

enum StreamType : uint32_t { kStreamTypeAudio = 0, kStreamTypeVideo = 1 };

// Structure provided to ContentDecryptionModule::OnPlatformChallengeResponse()
// after a platform challenge was initiated via Host::SendPlatformChallenge().
// All values will be NULL / zero in the event of a challenge failure.
struct PlatformChallengeResponse {
	// |challenge| provided during Host::SendPlatformChallenge() combined with
	// nonce data and signed with the platform's private key.
	const uint8_t* signed_data;
	uint32_t signed_data_length;

	// RSASSA-PKCS1-v1_5-SHA256 signature of the |signed_data| block.
	const uint8_t* signed_data_signature;
	uint32_t signed_data_signature_length;

	// X.509 device specific certificate for the |service_id| requested.
	const uint8_t* platform_key_certificate;
	uint32_t platform_key_certificate_length;
};

// The current status of the associated key. The valid types are defined in the
// spec: https://w3c.github.io/encrypted-media/#dom-mediakeystatus
enum KeyStatus : uint32_t {
	kUsable = 0,
	kInternalError = 1,
	kExpired = 2,
	kOutputRestricted = 3,
	kOutputDownscaled = 4,
	kStatusPending = 5,
	kReleased = 6
};


// Used when passing arrays of key information. Does not own the referenced
// data. |system_code| is an additional error code for unusable keys and
// should be 0 when |status| == kUsable.
struct KeyInformation {
	const uint8_t* key_id;
	uint32_t key_id_size;
	KeyStatus status;
	uint32_t system_code;
};

// Supported output protection methods for use with EnableOutputProtection() and
// returned by OnQueryOutputProtectionStatus().
enum OutputProtectionMethods : uint32_t {
	kProtectionNone = 0,
	kProtectionHDCP = 1 << 0
};

// Connected output link types returned by OnQueryOutputProtectionStatus().
enum OutputLinkTypes : uint32_t {
	kLinkTypeNone = 0,
	kLinkTypeUnknown = 1 << 0,
	kLinkTypeInternal = 1 << 1,
	kLinkTypeVGA = 1 << 2,
	kLinkTypeHDMI = 1 << 3,
	kLinkTypeDVI = 1 << 4,
	kLinkTypeDisplayPort = 1 << 5,
	kLinkTypeNetwork = 1 << 6
};


// Result of the QueryOutputProtectionStatus() call.
enum QueryResult : uint32_t { kQuerySucceeded = 0, kQueryFailed };


// The Initialization Data Type. The valid types are defined in the spec:
// https://w3c.github.io/encrypted-media/format-registry/initdata/index.html#registry
enum InitDataType : uint32_t { kCenc = 0, kKeyIds = 1, kWebM = 2 };


// The type of session to create. The valid types are defined in the spec:
// https://w3c.github.io/encrypted-media/#dom-mediakeysessiontype
enum SessionType : uint32_t {
	kTemporary = 0,
	kPersistentLicense = 1,
	kPersistentUsageRecord = 2
};


// The type of the message event.  The valid types are defined in the spec:
// https://w3c.github.io/encrypted-media/#dom-mediakeymessagetype
enum MessageType : uint32_t {
	kLicenseRequest = 0,
	kLicenseRenewal = 1,
	kLicenseRelease = 2,
	kIndividualizationRequest = 3
};


enum HdcpVersion : uint32_t {
	kHdcpVersionNone,
	kHdcpVersion1_0,
	kHdcpVersion1_1,
	kHdcpVersion1_2,
	kHdcpVersion1_3,
	kHdcpVersion1_4,
	kHdcpVersion2_0,
	kHdcpVersion2_1,
	kHdcpVersion2_2,
	kHdcpVersion2_3
};


struct Policy {
	HdcpVersion min_hdcp_version;
};


// Represents a buffer created by Allocator implementations.
class  Buffer {
public:
	// Destroys the buffer in the same context as it was created.
	virtual void Destroy();

	virtual uint32_t Capacity() const;
	virtual uint8_t* Data();
	virtual void SetSize(uint32_t size);
	virtual uint32_t Size() const;

protected:
	Buffer() {}
	virtual ~Buffer() {}

private:
	Buffer(const Buffer&);
	void operator=(const Buffer&);
};

// Represents a decrypted block that has not been decoded.
class  DecryptedBlock {
public:
	virtual void SetDecryptedBuffer(Buffer* buffer);
	virtual Buffer* DecryptedBuffer();

	// TODO(tomfinegan): Figure out if timestamp is really needed. If it is not,
	// we can just pass Buffer pointers around.
	virtual void SetTimestamp(int64_t timestamp);
	virtual int64_t Timestamp() const;

protected:
	DecryptedBlock() {}
	virtual ~DecryptedBlock() {}
};

enum VideoPlane : uint32_t {
	kYPlane = 0,
	kUPlane = 1,
	kVPlane = 2,
	kMaxPlanes = 3,
};


class  VideoFrame {
public:
	virtual void SetFormat(VideoFormat format);
	virtual VideoFormat Format() const;

	virtual void SetSize(Size size);
	virtual Size Size() const;

	virtual void SetFrameBuffer(Buffer* frame_buffer);
	virtual Buffer* FrameBuffer();

	virtual void SetPlaneOffset(VideoPlane plane, uint32_t offset);
	virtual uint32_t PlaneOffset(VideoPlane plane);

	virtual void SetStride(VideoPlane plane, uint32_t stride);
	virtual uint32_t Stride(VideoPlane plane);

	// Sets and gets the presentation timestamp which is in microseconds.
	virtual void SetTimestamp(int64_t timestamp);
	virtual int64_t Timestamp() const;

protected:
	VideoFrame() {}
	virtual ~VideoFrame() {}
};

// Represents a decoded video frame. The CDM should call the interface methods
// to set the frame attributes. See DecryptAndDecodeFrame().
class  VideoFrame_2 {
public:
	virtual void SetFormat(VideoFormat format);
	virtual void SetSize(Size size);
	virtual void SetFrameBuffer(Buffer* frame_buffer);
	virtual void SetPlaneOffset(VideoPlane plane, uint32_t offset);
	virtual void SetStride(VideoPlane plane, uint32_t stride);
	// Sets the presentation timestamp which is in microseconds.
	virtual void SetTimestamp(int64_t timestamp);
	virtual void SetColorSpace(ColorSpace color_space);

protected:
	VideoFrame_2() {}
	virtual ~VideoFrame_2() {}
};

// Represents decrypted and decoded audio frames. AudioFrames can contain
// multiple audio output buffers, which are serialized into this format:
//
// |<------------------- serialized audio buffer ------------------->|
// | int64_t timestamp | int64_t length | length bytes of audio data |
//
// For example, with three audio output buffers, the AudioFrames will look
// like this:
//
// |<----------------- AudioFrames ------------------>|
// | audio buffer 0 | audio buffer 1 | audio buffer 2 |
class  AudioFrames {
public:
	virtual void SetFrameBuffer(Buffer* buffer);
	virtual Buffer* FrameBuffer();

	// The CDM must call this method, providing a valid format, when providing
	// frame buffers. Planar data should be stored end to end; e.g.,
	// |ch1 sample1||ch1 sample2|....|ch1 sample_last||ch2 sample1|...
	virtual void SetFormat(AudioFormat format);
	virtual AudioFormat Format() const;

protected:
	AudioFrames() {}
	virtual ~AudioFrames() {}
};

// FileIO interface provides a way for the CDM to store data in a file in
// persistent storage. This interface aims only at providing basic read/write
// capabilities and should not be used as a full fledged file IO API.
// Each CDM and origin (e.g. HTTPS, "foo.example.com", 443) combination has
// its own persistent storage. All instances of a given CDM associated with a
// given origin share the same persistent storage.
// Note to implementors of this interface:
// Per-origin storage and the ability for users to clear it are important.
// See http://www.w3.org/TR/encrypted-media/#privacy-storedinfo.
class  FileIO {
public:
	// Opens the file with |file_name| for read and write.
	// FileIOClient::OnOpenComplete() will be called after the opening
	// operation finishes.
	// - When the file is opened by a CDM instance, it will be classified as "in
	//   use". In this case other CDM instances in the same domain may receive
	//   kInUse status when trying to open it.
	// - |file_name| must only contain letters (A-Za-z), digits(0-9), or "._-".
	//   It must not start with an underscore ('_'), and must be at least 1
	//   character and no more than 256 characters long.
	virtual void Open(const char* file_name, uint32_t file_name_size);

	// Reads the contents of the file. FileIOClient::OnReadComplete() will be
	// called with the read status. Read() should not be called if a previous
	// Read() or Write() call is still pending; otherwise OnReadComplete() will
	// be called with kInUse.
	virtual void Read();

	// Writes |data_size| bytes of |data| into the file.
	// FileIOClient::OnWriteComplete() will be called with the write status.
	// All existing contents in the file will be overwritten. Calling Write() with
	// NULL |data| will clear all contents in the file. Write() should not be
	// called if a previous Write() or Read() call is still pending; otherwise
	// OnWriteComplete() will be called with kInUse.
	virtual void Write(const uint8_t* data, uint32_t data_size);

	// Closes the file if opened, destroys this FileIO object and releases any
	// resources allocated. The CDM must call this method when it finished using
	// this object. A FileIO object must not be used after Close() is called.
	virtual void Close();

protected:
	FileIO() {}
	virtual ~FileIO() {}
};

// Responses to FileIO calls. All responses will be called asynchronously.
// When kError is returned, the FileIO object could be in an error state. All
// following calls (other than Close()) could return kError. The CDM should
// still call Close() to destroy the FileIO object.
class  FileIOClient {
public:
	enum class Status : uint32_t { kSuccess = 0, kInUse, kError };

	// Response to a FileIO::Open() call with the open |status|.
	virtual void OnOpenComplete(Status status);

	// Response to a FileIO::Read() call to provide |data_size| bytes of |data|
	// read from the file.
	// - kSuccess indicates that all contents of the file has been successfully
	//   read. In this case, 0 |data_size| means that the file is empty.
	// - kInUse indicates that there are other read/write operations pending.
	// - kError indicates read failure, e.g. the storage is not open or cannot be
	//   fully read.
	virtual void OnReadComplete(Status status,
		const uint8_t* data,
		uint32_t data_size);

	// Response to a FileIO::Write() call.
	// - kSuccess indicates that all the data has been written into the file
	//   successfully.
	// - kInUse indicates that there are other read/write operations pending.
	// - kError indicates write failure, e.g. the storage is not open or cannot be
	//   fully written. Upon write failure, the contents of the file should be
	//   regarded as corrupt and should not used.
	virtual void OnWriteComplete(Status status);

protected:
	FileIOClient() {}
	virtual ~FileIOClient() {}
};


class ContentDecryptionModule_10 {
private:
	ContentDecryptionModule_10 *orig_cdm;


public:



	virtual  void Initialize(bool allow_distinctive_identifier,
		bool allow_persistent_state,
		bool use_hw_secure_codecs) {
		
	
	}


	virtual  void GetStatusForPolicy(uint32_t promise_id,
		const Policy& policy) {}

	virtual  void SetServerCertificate(uint32_t promise_id,
		const uint8_t* server_certificate_data,
		uint32_t server_certificate_data_size) {}

	virtual  void CreateSessionAndGenerateRequest(uint32_t promise_id,
		SessionType session_type,
		InitDataType init_data_type,
		const uint8_t* init_data,
		uint32_t init_data_size) {}

	virtual  void LoadSession(uint32_t promise_id,
		SessionType session_type,
		const char* session_id,
		uint32_t session_id_size) {}

	void UpdateSession(uint32_t promise_id,
		const char* session_id,
		uint32_t session_id_size,
		const uint8_t* response,
		uint32_t response_size) {}

	virtual  void CloseSession(uint32_t promise_id,
		const char* session_id,
		uint32_t session_id_size) {}

	virtual  void RemoveSession(uint32_t promise_id,
		const char* session_id,
		uint32_t session_id_size) {}

	virtual void TimerExpired(void* context) {}

	virtual Status Decrypt( InputBuffer_2& encrypted_buffer,
		DecryptedBlock* decrypted_buffer) {}

	virtual Status InitializeAudioDecoder(
		const AudioDecoderConfig_2& audio_decoder_config) {}

	virtual Status InitializeVideoDecoder(
		const VideoDecoderConfig_2& video_decoder_config) {}

	virtual void DeinitializeDecoder(StreamType decoder_type) {}

	virtual void ResetDecoder(StreamType decoder_type) {}

	virtual Status DecryptAndDecodeFrame(const InputBuffer_2& encrypted_buffer,
		VideoFrame* video_frame) {

	}

	virtual Status DecryptAndDecodeSamples(const InputBuffer_2& encrypted_buffer,
		AudioFrames* audio_frames) {

	}

	virtual void OnPlatformChallengeResponse(
		const PlatformChallengeResponse& response) {

	}

	virtual void OnQueryOutputProtectionStatus(
		QueryResult result,
		uint32_t link_mask,
		uint32_t output_protection_mask) {

	}

	virtual void OnStorageId(uint32_t version,
		const uint8_t* storage_id,
		uint32_t storage_id_size) {}

	void Destroy() {

	}


};

