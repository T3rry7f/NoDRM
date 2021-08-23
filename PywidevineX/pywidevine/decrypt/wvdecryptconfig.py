import pywidevine.downloader.wvdownloaderconfig as wvdl_cfg

class WvDecryptConfig(object):
    def __init__(self, filename, tracktype, trackno, license, init_data_b64, cert_data_b64=None):
        self.filename = filename
        self.tracktype = tracktype
        self.trackno = trackno
        self.init_data_b64 = init_data_b64
        self.license = license
        if cert_data_b64 is not None:
            self.server_cert_required = True
            self.cert_data_b64 = cert_data_b64
        else:
            self.server_cert_required = False

    def get_filename(self, unformatted_filename):
        return unformatted_filename.format(filename=self.filename, track_type=self.tracktype, track_no=self.trackno)

    def build_commandline_list(self, keys):
        commandline = [wvdl_cfg.MP4DECRYPT_BINARY_PATH]
        for key in keys:
            print(key)
            if key.type == 'CONTENT':
                commandline.append('--key')
                commandline.append('{}:{}'.format(key.kid.hex(), key.key.hex()))
        commandline.append(self.get_filename(wvdl_cfg.ENCRYPTED_FILENAME))
        commandline.append(self.get_filename(wvdl_cfg.DECRYPTED_FILENAME))
        return commandline
