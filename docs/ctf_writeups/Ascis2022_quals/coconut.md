# Coconut

Challenges là một file exe viết bằng C#: `Coconut.exe: PE32+ executable (console) x86-64 Mono/.Net assembly, for MS Windows`

# Load lên bằng dnspy và phân tích

![image](https://user-images.githubusercontent.com/31529599/196735307-f0e201d7-c713-476c-a479-9780998d8b61.png)

Đây là code hàm main của chương trình, về cơ bản chương trình sẽ gọi hàm `coconut_10()` để khởi tạo một số giá trị của mảng và dictionary 

![image](https://user-images.githubusercontent.com/31529599/196735578-39c2de6b-6d43-4b2f-8b2b-94cc2fb53c15.png)

![image](https://user-images.githubusercontent.com/31529599/196735839-cf756dc0-acca-457f-a7fc-3d48911da178.png)

Tiếp theo đó gọi hàm `coconut_28()` và hàm này sẽ gọi đến hàm `coconut_82()`  tuy nhiên ta dnspy không thể decompile được hàm `coconut_82`. Vì vậy nên luồng của chuơng trình sẽ lỗi và nhảy vào catch exception của hàm `coconut_28` và thực thi hàm `coconut_25` 

```C#
		public static object coconut_25(InvalidProgramException e, object[] args, Dictionary<uint, int> m, byte[] b)
		{
			int metadataToken = new StackTrace(e).GetFrame(0).GetMethod().MetadataToken;
			Module module = typeof(Program).Module;
			MethodInfo methodInfo = (MethodInfo)module.ResolveMethod(metadataToken);
			MethodBase methodBase = module.ResolveMethod(metadataToken);
			ParameterInfo[] parameters = methodInfo.GetParameters();
			Type[] array = new Type[parameters.Length];
			SignatureHelper localVarSigHelper = SignatureHelper.GetLocalVarSigHelper();
			for (int i = 0; i < array.Length; i++)
			{
				array[i] = parameters[i].ParameterType;
			}
			Type declaringType = methodBase.DeclaringType;
			DynamicMethod dynamicMethod = new DynamicMethod("", methodInfo.ReturnType, array, declaringType, true);
			DynamicILInfo dynamicILInfo = dynamicMethod.GetDynamicILInfo();
			MethodBody methodBody = methodInfo.GetMethodBody();
			foreach (LocalVariableInfo localVariableInfo in methodBody.LocalVariables)
			{
				localVarSigHelper.AddArgument(localVariableInfo.LocalType);
			}
			byte[] signature = localVarSigHelper.GetSignature();
			dynamicILInfo.SetLocalSignature(signature);
			foreach (KeyValuePair<uint, int> keyValuePair in m)
			{
				int value = keyValuePair.Value;
				uint key = keyValuePair.Key;
				int tokenFor;
				if (value >= 1879048192 && value < 1879113727)
				{
					tokenFor = dynamicILInfo.GetTokenFor(module.ResolveString(value));
				}
				else
				{
					MemberInfo memberInfo = declaringType.Module.ResolveMember(value, null, null);
					if (memberInfo.GetType().Name == "RtFieldInfo")
					{
						tokenFor = dynamicILInfo.GetTokenFor(((FieldInfo)memberInfo).FieldHandle, ((TypeInfo)((FieldInfo)memberInfo).DeclaringType).TypeHandle);
					}
					else if (memberInfo.GetType().Name == "RuntimeType")
					{
						tokenFor = dynamicILInfo.GetTokenFor(((TypeInfo)memberInfo).TypeHandle);
					}
					else if (memberInfo.Name == ".ctor" || memberInfo.Name == ".cctor")
					{
						tokenFor = dynamicILInfo.GetTokenFor(((ConstructorInfo)memberInfo).MethodHandle, ((TypeInfo)((ConstructorInfo)memberInfo).DeclaringType).TypeHandle);
					}
					else
					{
						tokenFor = dynamicILInfo.GetTokenFor(((MethodInfo)memberInfo).MethodHandle, ((TypeInfo)((MethodInfo)memberInfo).DeclaringType).TypeHandle);
					}
				}
				b[(int)key] = (byte)tokenFor;
				b[(int)(key + 1U)] = (byte)(tokenFor >> 8);
				b[(int)(key + 2U)] = (byte)(tokenFor >> 16);
				b[(int)(key + 3U)] = (byte)(tokenFor >> 24);
			}
			dynamicILInfo.SetCode(b, methodBody.MaxStackSize);
			return dynamicMethod.Invoke(null, args);
		}
```
Trên đây là code của hàm `coconut_25` cơ bản hàm này sẽ cố gắng sửa lại code của hàm bị lỗi (coconut_82) và sử dụng hàm `dynamicMethod.Invoke` để gọi code vừa được tạo (`b`)

Nghĩa là code đúng của hàm `coconut_82` sẽ là giá trị của mảng `b` được truyền vào hàm `SetCode`, tuy nhiên có một vấn đề nếu như ta cố gắng copy code này và patch vào vị trí của hàm `coconut_82` rồi sử dụng `dnspy` để decompile thì code vẫn sẽ lỗi

Lí do cơ bản là bởi vì hàm `dynamicMethod.Invoke` sử dụng token khác với code đúng khi đặt ở text sections, đó cũng là lí do chương trình cố gắng lấy token mới cho code `b` bằng hàm `GetTokenFor`. Vậy nghĩa là token đúng sẽ là token ban đầu trước khi gọi hàm `GetTokenFor` -> index,token đúng tương ứng với key, value

Từ đây ta chỉ cần build lại code đúng cho hàm `coconut_82` từ index và token đã xác định. Ngoài hàm `coconut_82` thì còn 9 hàm khác cũng có luồng chương trình tương tự như hàm này nên ta tiến hành trích cách code đúng cho từng hàm và patch lại cho đúng

Code patch:

```python
from pyexpat.errors import codes


def patch_file(exe,patch_bytes,offset):
    orginal = exe[offset:offset+len(patch_bytes)]
    exe = exe.replace(orginal,patch_bytes)
    return exe

def build_code(key_vals,code_original):
    code_arr = [i for i in code_original]
    for k in key_vals:
        # print(k,key_vals[k])
        code_arr[k]=key_vals[k]&0xff
        code_arr[k+1]=(key_vals[k]>>8)&0xff
        code_arr[k+2]=(key_vals[k]>>16)&0xff
        code_arr[k+3]=(key_vals[k]>>24)&0xff

def get_b():
    cnt = open('b_code.txt','r')
    arr = '['+cnt.read()+']'
    return eval(arr)

f = open('./Coconut/Coconut.exe','rb')
exe = f.read()

a1 = b'r\x01\x00\x00p(\x04\x00\x00\n(\x05\x00\x00\n*'
a2 = b'r\x19\x00\x00p\n\x02\x0b\x16\x0c8?\x00\x00\x00\x07\x08\x91\r\t\x1f\n<\x1a\x00\x00\x00\x06\t\x1f0X\xd1\x13\x04\x12\x04(N\x00\x00\n(7\x00\x00\n\n8\x15\x00\x00\x00\x06\t\x1fWX\xd1\x13\x04\x12\x04(N\x00\x00\n(7\x00\x00\n\n\x08\x17X\x0c\x08\x07\x8ei2\xbb\x06 \x03\x02\x00\x00(?\x00\x00\n*'
a3 = b'(\x1e\x00\x00\x06\x02(\x12\x00\x00\x06*'
a4 = b'(<\x00\x00\n\x02o3\x00\x00\n(=\x00\x00\nr\xc5\x00\x00pr\x19\x00\x00po>\x00\x00\n*'
a5 = b'(2\x00\x00\n\x03o3\x00\x00\n\n(2\x00\x00\nr\xc9\x00\x00po3\x00\x00\n\x0b\x029\x07\x00\x00\x00\x02\x8e:\x01\x00\x00\x00*\x069\x07\x00\x00\x00\x06\x8e:\x01\x00\x00\x00*\x079\x07\x00\x00\x00\x07\x8e:\x01\x00\x00\x00*\x02\x8ei\x8d\x02\x00\x00\x01\x0csC\x00\x00\n\r\t\x06oD\x00\x00\n\t\x07oE\x00\x00\n\t\toF\x00\x00\n\toG\x00\x00\noH\x00\x00\n\x13\x04\x02sI\x00\x00\n\x13\x05\x11\x05\x11\x04\x16sJ\x00\x00\n\x13\x06\x11\x06sK\x00\x00\n\x13\x07\x11\x07oL\x00\x00\n\x08\x16\x02\x8eioM\x00\x00\n&\xdd:\x00\x00\x00\x11\x079\x07\x00\x00\x00\x11\x07o\x1b\x00\x00\n\xdc\x11\x069\x07\x00\x00\x00\x11\x06o\x1b\x00\x00\n\xdc\x11\x059\x07\x00\x00\x00\x11\x05o\x1b\x00\x00\n\xdc\t9\x06\x00\x00\x00\to\x1b\x00\x00\n\xdcr\xeb\x00\x00p\x08(4\x00\x00\n(\x0c\x00\x00\x06*'
a6 = b'(8\x00\x00\n\n\x12\x00(9\x00\x00\n \xce\x0b\x00\x00<\x0f\x00\x00\x00r\x8b\x00\x00p(:\x00\x00\n8\x06\x00\x00\x00(\x08\x00\x00\x06* \x00\\&\x05(;\x00\x00\n+\xc8'
a7 = b's5\x00\x00\n\n\x06\x18o\x07\x00\x00\no\x08\x00\x00\no6\x00\x00\n\x06\x17o\x07\x00\x00\no\x08\x00\x00\no6\x00\x00\n(7\x00\x00\n*'
a8 = b'(\x1d\x00\x00\x06\n(\n\x00\x00\x06\x0b(2\x00\x00\n\x07o3\x00\x00\n\x06(\x06\x00\x00\x06\x0cre\x00\x00p\x08(4\x00\x00\n*'
a9 = b' \x00\x01\x00\x00\x8d"\x00\x00\x01\n \x00\x01\x00\x00\x8d"\x00\x00\x01\x0b\x03\x8ei\x8d\x02\x00\x00\x01\x0c\x16\r8\x12\x00\x00\x00\x06\t\x02\t\x02\x8ei]\x91\x9e\x07\t\t\x9e\t\x17X\r\t \x00\x01\x00\x002\xe6\x16%\x13\x04\r8(\x00\x00\x00\x11\x04\x07\t\x94X\x06\t\x94X \x00\x01\x00\x00]\x13\x04\x07\t\x94\x13\x05\x07\t\x07\x11\x04\x94\x9e\x07\x11\x04\x11\x05\x9e\t\x17X\r\t \x00\x01\x00\x002\xd0\x16%\r%\x13\x06\x13\x048X\x00\x00\x00\x11\x06\x17X\x13\x06\x11\x06 \x00\x01\x00\x00]\x13\x06\x11\x04\x07\x11\x06\x94X\x13\x04\x11\x04 \x00\x01\x00\x00]\x13\x04\x07\x11\x06\x94\x13\x07\x07\x11\x06\x07\x11\x04\x94\x9e\x07\x11\x04\x11\x07\x9e\x07\x07\x11\x06\x94\x07\x11\x04\x94X \x00\x01\x00\x00]\x94\x13\x08\x08\t\x03\t\x91\x11\x08a\xd2\x9c\t\x17X\r\t\x03\x8ei2\xa2\x08*'
a10 = b'\x02 \x03\x02\x00\x00(?\x00\x00\n~\n\x00\x00\x04(\x16\x00\x00\x06(@\x00\x00\n~\x0b\x00\x00\x04(\x16\x00\x00\x06(A\x00\x00\n~\x0c\x00\x00\x04(\x16\x00\x00\x06(B\x00\x00\n9\x02\x00\x00\x00\x17*\x16*'
codes  = [a1,a2,a3,a4,a5,a6,a7,a8,a9,a10]

offsets = [0x000007D0+0x1,0x00001134+0xc,0x000010DC+0x1,0x00000E40+0x1,0x00000F58+0xc,
0x00000DB4+0xc,0x00000D28+0xc,0x00000CA8+0xc,0x00000B5C+0xc,0x00000EB4+0xc
]


for i in range(len(codes)):
    print(codes[i])
    exe = patch_file(exe,codes[i],offsets[i])
print(len(offsets))
open('patched.exe','wb').write(exe)
```
 
 - mảng codes lần lượt là code đúng cho tường hàm
 - offsets là vị trí patch của từng hàm tương ứng với mảng codes

## Load code sau khi deobfuscate vào dnspy

![image](https://user-images.githubusercontent.com/31529599/196739428-8b13d763-5bc5-476f-b724-bf6870de9a06.png)

Sau khi patch lại code đúng ta biết được hàm `coconut_82` sẽ yêu cầu chúng ta nhập vào 1 `key` gì đó

![image](https://user-images.githubusercontent.com/31529599/196739769-66f1109b-668a-4511-a6da-9bb4d72adee8.png)

Sau khi nhập vào, input sẽ truyền vào hàm `coconut_15` để chuyển sang dạng hex

![image](https://user-images.githubusercontent.com/31529599/196740064-c4de5159-5e84-49fb-9b3d-3d6ed1a9c518.png)

Tiếp theo truyền vào hàm `coconut_25` để kiểm tra `key`, ở đây ta thấy biểu thức cơ bản `key_input*a1%a2==a3`

tiến hành debug và dump ra từ hằng số ta được:
- `a1` = 13880328245145378785705410836767432968611776023726110634003371836796654391468
- `a2` = 94681236187809407884249877636270988187726197118196720312520904250574148078753
- `a3` = 84691773930568242145085772534897353407834382467231062104573594077086839033728

=> `input = a3 * pow(a1, -1, a2) % a2`

input = 40461490530359865778886961981237321600838390469839338807592079184775945025825 => `Ytd_is_history_Tmr_is_a_mystery!`

Input sau khi được check đúng sẽ truyền vào hàm `coconut_54` như một key aes để decypt một hình png

![image](https://user-images.githubusercontent.com/31529599/196747183-5ea05b85-a4c2-4781-9dd3-05c8ac6035d0.png)

![image](https://user-images.githubusercontent.com/31529599/196747871-d4102f4c-2161-45d1-86ea-49e0452a0315.png)

Nhập key và ta được 1 nữa flag từ hình mới decrypt tiếp tục phân tích phase tiếp theo 

![image](https://user-images.githubusercontent.com/31529599/196748266-cf20ede4-2ee0-45dd-bd05-94a49497ed92.png)

Phần còn lại của flag nằm ở hàm `coconut_63`, hàm này cũng sẽ decrypt một hình png thông qua hàm `coconut_26` sử dụng thuật toán RC4 với key được tạo từ hàm `coconut_16` 

![image](https://user-images.githubusercontent.com/31529599/196748648-c46deb2e-1d47-4c25-819b-e5662c2823e1.png)

Hàm `coconut_16` sẽ lấy các hàm trên stacktrace ở vị trí 2 và 1 sau đó cộng lại, để lấy chính xác giá trị của key ta nên sử dụng chương trình ban đầu sau đó debug và lấy giá trị trả về của hàm `coconut_16`

Key chính xác sẽ là: `System.Object Invoke(System.Object, System.Reflection.BindingFlags, System.Reflection.Binder, System.Object[], System.Globalization.CultureInfo)System.Object InvokeMethod(System.Object, System.Object[], System.Signature, Boolean)`

Decrypt hình png:

```python
from Crypto.Cipher import ARC4
key = b"System.Object Invoke(System.Object, System.Reflection.BindingFlags, System.Reflection.Binder, System.Object[], System.Globalization.CultureInfo)System.Object InvokeMethod(System.Object, System.Object[], System.Signature, Boolean)"
cipher = ARC4.new(key)
cnt = open('DRAGON_WARRIOR','rb').read()

open('image.png','wb').write(cipher.decrypt(cnt))

```

Phần còn lại của flag:

![image](https://user-images.githubusercontent.com/31529599/196749204-9f3bc813-55ec-4a84-9579-b40188d4f425.png)




