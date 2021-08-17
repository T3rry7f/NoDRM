#!/usr/local/bin/python
#coding: utf-8
__author__ = 'T3rry'
import re
import json
import base64
import requests
import sys,os
from socket  import *
from Crypto.Signature import pss
from Crypto.Hash import SHA1
from Crypto.Cipher import PKCS1_v1_5 as Cipher_pkcs1_v1_5
from Crypto.Signature import PKCS1_v1_5 as Signature_pkcs1_v1_5
from Crypto.Cipher import PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Hash import CMAC
from Crypto.Cipher import AES
import license_protocol_pb2
import binascii


CONFIG_FILE='wvConfig.json'
LICENSE_PROXY_SERVER=None  # widevine proxy server
LICENSE_SERVER_GATEWAY=None   # widevine license server

class WidevineCDM:
    def __init__(self,url):
        #self.private_key='308204a30201000282010100b5d1dc441883596c5d2722832d33cef4e4daa6e9959d6fbd83a9374527e533408448512e7d9509182ef750a7bd7bebbbf3d1d5653d38a41e68af7581d173b168e89b26494b06477b61f9f53a7755ade9cc293135178ffa8e0e6b9b0cafe2a150d6ef0cfd385952b0206fca5398a7dbf6faefd55f00029c15cdc420dece3c7844a72a3054f7d564f1a94f4e33d27ce8284c396e1b140e3568b009a3307ed36c62b3b395d7be57750e6f9155ccf72b3a668445fcae8d5de1e2c1c645b4c2b2a615c0c6a53bb866366b5e9b0b74c41b9fe49ba26bbb75b1cb89ca943c948d6212c07e259568dd4a2f7daf67357d209794c0ab5b4087a339e7fb6da56022ad61ef0902030100010282010018e3c326f1421dde373c51bdaa54f2ca547fd82496ce280b45f846b0295f776e280dac4b5476aff98708651aa9564af57e51a5c847a2b6d8d0d4e01da6da1319bce9ec4a5142694bab24681d1a53f8cc4e1dff75f8a54593e7c67441bedc23e028a42ddf8634b81c933c2a72da2d746fb1775e7ab44a272ad6f1b7dc38584fd03f0d122362bf18d00568bead150e35aa035156e4e3bda7bbe4cba7be3c3323487b9c43eb9a2f355949ceef58e874b47e4cd06564d7b62906207f893a70e3305421c6a77905f779a21f4820c72b44820fa21117b925fae391cef5aa896946ce9746d81f7abb23f885ddd6a0f7199ed33bf4f2a6e1d028b5d8266a56ee78525fa702818100d4cb413203d16ad1a3c5e0b2031ea0cf76e226e2110065feac40b77c15eeb2c0ce29f4a384571ed83da1714071528088965ecc2295d3c997c0a0e5c336132314d6d767e71691e1520393c7b62440df84fb5a5ac929269dab536c07bc05ce780112ed414cae484a56aa9539d6e822194b75c629e4ac622779b020d4923bde128702818100dabc9bc5f9cf0020d7c268ca1c517d249d7cfec42c1d3f8df41a83d00876b5ad48f96d9dea9f75ccc1259ae7b278c77a558589a026fe23a442be2c150b15eccd2d5e4de02eef1fadda668e0e17b21479d1414b9079d3cc80abb4623e137654e0bc2d1743879a2a9c5b9f8da7c5f36cbd77efdccc2ad5206e370fe28eda3e05ef028180383f1d9585d2d60461e0cd1ae09e38ed7dc41b7907fb6dfa5a37a5086497baa2221c8ef0a5eb8d58a539c640bd738c4c0e4b327435dc4c5e1369b431dc5a449c9e89438a9eb9a2b05607baf35733daa140fb4a220001980d90386ef6f125f92c777f45126ac2eafb6b8d94434d0aae5af6df91754367927da4e398acaaac7183028180393adddae7a86455337e7722625463d4bfabe3907a2650e9983393c74b5f9bdb31dba8f5875c9f5aaa32679c3592ea4634b812b1276298fab247c58adff2a5996d445e45c8a1e1fcffc693665686ce5aad08537802980acaa3a2378e1c537a93ae4871ecc63eece52a07cded569a8119f5967983a5b54b9deaa42a57cbfc2c5b028181009ceabe2ecf3709a1c85828e955f8960be47b9aa5beaa5d4e1ada1a6a3b40e00ce15f35fc1c85e9623ba93c1957950d4515f3de9ba8f06b551365ff02a486fca4f50b00df5946bc46f15f9bbe465655110f4d98fbc4f0b03da64734aa009a2dc36efed2e521180db057fcdc8f08b138b23fc08133db52c52d6a2c394efacfb051'.decode('hex')
        self.pub_key=binascii.a2b_hex('30820122300d06092a864886f70d01010105000382010f003082010a0282010100bca83d793f493c49df558612e74c773198ab4901f20369bfaf1598d71e362ef13ab9be3b4d4d73c63378542d23beba56ad4d589c1e7f151e25cf6f7a38f8ff1ff491d5d2dfc971617b6d9559406e3a5127b2aebddea965e0dfcf4c50ae241caf9e87bfe33b0db619b5c395e3986e310a3278f990b4139a421af74b3e4e1548250dec8f1755b038e61069e2547983ed93878549b4a9f5faa1bef72a75a9929fa7240fb1e46b9587170ef993c29c35f1f145e55bfec0de85d2b9409d6599b1c348bf76dd441abd53033475e3267f91647c2584d974d3ad7b8c0c33711556d6c2cf23bf7905b17a68c622a0580a623c1af9f446294d5f2de50721d85eb5f49b70130203010001')
        self.license_proxy_address=LICENSE_PROXY_SERVER
        self.license_request_data=None
        
        self.header={}
        self.sock=socket(AF_INET,SOCK_STREAM)
        self.pssh_data=None
        self.kid=None
        self.pssh_type=None
        self.sessionKey=None
        self.responseMessage=None
        self.requestMessage=None
        self.sessions=[]
        self.LoadConfig()
        self.license_url=url 

    def LoadConfig(self):

        if(os.path.exists(CONFIG_FILE)):
            config=json.loads(open(CONFIG_FILE).read())
            ipaddress=config['LICENSE_PROXY_SERVER'].split(':')
            self.pssh_type=config['PSSH_CONTAINER']
            self.license_proxy_address=tuple([ipaddress[0],int(ipaddress[1])])
            self.license_url=config['LICENSE_SERVER']
            self.header=config['HEADER']

            #self.sock.connect(self.license_proxy_address)
        else:
            print("wvConfig.json is missing")
            return

    def GetServerCertificate(self):
        data=b'\x08\x04'
        cert=requests.post(self.license_url,data,headers=self.header).content
        return cert[5:]

    def SetServerCertificate(self):
        data=b'\x08\x04'
        cert=requests.post(self.license_url,data,headers=self.header).content
        #self.sock.send(cert[5:])

    def CreateSessionAndGenerateRequest(self,url): # 
        
        resp=self.Download(url)
        self.ParsePSSH(resp)
        self.sock.send(self.pssh_data)
        recvData=self.sock.recv(10240)
        self.license_request_data=recvData
        #print 'License request data: \n',recvData.encode('hex')
        return recvData

    def ParsePSSH(self,data):

        self.content=data
        
        if(self.pssh_type=='mpd'): # parse mpd file 
            try:

                matchObj = re.findall(r'ed.+\n.+(pssh.+)<', data.decode('utf-8'), re.M | re.I)
                pssh_data= matchObj[0].split('>')[1].split('<')[0]
                self.pssh_data=base64.b64decode(pssh_data)
            except Exception as e:
               print(e)
               pass

        elif(self.pssh_type=='mp4'): # parse mp4 init data
            try:
                offset=self.content.rfind(rb'pssh')
                length=self.content[offset-1] #py3
                #length= ord(self.content[offset-1]) #py2
                self.pssh_data =self.content[offset-4:offset+length-4]
                #print(self.pssh_data)
                #self.kid=self.content[offset-4+2+32:offset-4+32+2+16].encode('hex')
            except Exception as e:
                print(e)
                pass
        elif(self.pssh_type=='base64'): # base64 pssh string
            try:
                self.pssh_data= base64.b64decode(data)
            except Exception as e:
                print(e)
                pass

    def GetSessionKey(self,lic_request_data):
        license=license_protocol_pb2.License()

        self.requestMessage=license_protocol_pb2.SignedMessage()
        self.requestMessage.ParseFromString(lic_request_data)
        self.responseMessage = license_protocol_pb2.SignedMessage()
        
        #print(responseMessage)
        license_request_msg=license_protocol_pb2.LicenseRequest()
        license_request_msg.ParseFromString(self.requestMessage.msg)
        request_time=license_request_msg.request_time

        #print(request_time)
        #print license_request_msg

        # device_cert_msg=license_protocol_pb2.DeviceCertificate()
        # device_cert_msg.ParseFromString(license_request_msg.client_id.token)
        #print(device_cert_msg.type)
        # print(device_cert_msg.serial_number.encode('hex'))
        # print(device_cert_msg.public_key.encode('hex'))
        #print(device_cert_msg.system_id)
        #print(license_request_msg)

        self.requestMessage.msg=license_request_msg.SerializeToString()
        resp=requests.post(self.license_url,lic_request_data,headers=self.header)
        self.responseMessage.ParseFromString(resp.content)
        #print(resp.content)
        
        pubkey = RSA.importKey(self.pub_key)
        verifier = pss.new(pubkey)
        h = SHA1.new(self.requestMessage.msg)
        verifier.verify(h, self.requestMessage.signature)
        enc_session_key=(self.responseMessage.session_key)
        return binascii.b2a_hex(enc_session_key)

  
    def GetContentKey(self,lic_request_data):
        #self.sessionKey=cipher.decrypt(enc_session_key)
        license=license_protocol_pb2.License()

        self.requestMessage=license_protocol_pb2.SignedMessage()
        self.requestMessage.ParseFromString(lic_request_data)
        self.responseMessage = license_protocol_pb2.SignedMessage()
        
        #print(responseMessage)
        license_request_msg=license_protocol_pb2.LicenseRequest()
        license_request_msg.ParseFromString(self.requestMessage.msg)
        #print license_request_msg

        # device_cert_msg=license_protocol_pb2.DeviceCertificate()
        # device_cert_msg.ParseFromString(license_request_msg.client_id.token)
        #print(device_cert_msg.type)
        # print(device_cert_msg.serial_number.encode('hex'))
        # print(device_cert_msg.public_key.encode('hex'))
        #print(device_cert_msg.system_id)
        #print(license_request_msg)
        RequestTime=license_request_msg.RequestTime

        #print(RequestTime)

        self.requestMessage.msg=license_request_msg.SerializeToString()
        resp=requests.post(self.license_url,lic_request_data,headers=self.header)
        self.responseMessage.ParseFromString(resp.content)
        #print(resp.content)
        
        pubkey = RSA.importKey(self.pub_key)
        verifier = pss.new(pubkey)
        h = SHA1.new(self.requestMessage.msg)
        verifier.verify(h, self.requestMessage.signature)
        enc_session_key=binascii.b2a_hex(self.responseMessage.session_key)
        #print(enc_session_key.encode('hex'))
        self.sock.send(enc_session_key)#
        self.sessionKey=binascii.a2b_hex(self.sock.recv(1024))  
        license.ParseFromString(self.responseMessage.msg)
        context_enc=b'\x01ENCRYPTION\x00'+self.requestMessage.msg+b'\x00\x00\x00\x80'
        mode = AES.MODE_CBC
        cobj = CMAC.new(self.sessionKey, ciphermod=AES)
        decryptKey=cobj.update(context_enc).digest()

        for k in license.key:
            keyId = binascii.b2a_hex(k.id).decode('utf-8')
            keyData = k.key[0:16]
            keyIv = k.iv[0:16]
            if(keyId):
                cryptos = AES.new(decryptKey, mode, keyIv)
                dkey = binascii.b2a_hex(cryptos.decrypt(keyData)).decode('utf-8')
                print("KID:%s KEY:%s"%(keyId,dkey))

    def GetContentKey2(self):
        #sessionKey=cipher.decrypt(enc_session_key)
        #print(self.sessionKey)
        license=license_protocol_pb2.License()
        license.ParseFromString(self.responseMessage.msg)
        context_enc=b'\x01ENCRYPTION\x00'+self.requestMessage.msg+b'\x00\x00\x00\x80'
        mode = AES.MODE_CBC
        cobj = CMAC.new(binascii.a2b_hex(self.sessionKey), ciphermod=AES)
        decryptKey=cobj.update(context_enc).digest()

        output=''
        for k in license.key:
            keyId = binascii.b2a_hex(k.id).decode('utf-8')
            keyData = k.key[0:16]
            keyIv = k.iv[0:16]
            
            if(keyId):
                cryptos = AES.new(decryptKey, mode, keyIv)
                dkey = binascii.b2a_hex(cryptos.decrypt(keyData)).decode('utf-8')
                print("KID:%s KEY:%s"%(keyId,dkey))
                output +=f"KID:{keyId} KEY:{dkey}\n"
        

        return output

    def Download(self,url):

        if(self.pssh_type=='mpd'):
            resp=requests.get(url,headers=self.header)
            #print(resp.content)
            return  resp.content        

        elif(self.pssh_type=='mp4'):
            self.header['Range']='bytes=0-4096'
            resp=requests.get(url,headers=self.header)
            return  resp.content        

        elif(self.pssh_type=='base64'):
            return url



if __name__ == '__main__':

    if len(sys.argv)>1:
        url=sys.argv[1]

        cdm=WidevineCDM(LICENSE_PROXY_SERVER) 
        #cdm.SetServerCertificate()
        data=cdm.CreateSessionAndGenerateRequest(url) 
        cdm.GetContentKey(data)
    else:
        print("Please input the  link")


