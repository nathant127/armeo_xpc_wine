Packets come in multiple types, these don't seem to be indicated in the sent data

Bulk Buffer may include an extra 4 bytes at the end, not sure what purpose this serves yet.

Payload format:
0: 0x4e
1: 0xcb
2: cmd
3: sub_cmd
4-7: size
8-End: data

OR 2 Payload Format:

Main Payload (first)
0: 0x4e
1: 0xcb
2: cmd
3: sub_cmd
4-7: size
8+: data

Bulk Payload (second) (should be directly concatenated to the end of the main payload for interpretation)
0+: data

The data takes a format specified by cmd and sub_cmd
This is defined internally using a format string for both request and response
Note that if a section of the payload has a dynamic size (ie string or array) there will be an integer prefix at the start that determines the size of the string/array



2 Payload format typically used for larger payloads

Major type is a simplified overview of the contents:
0x14 = INT
0xc8 = VOID (multiple types)
etc.

Format ID is an id that refers to a specific payload format associated with the major type.
A table exists in memory that contains the format for possible major_type + fmt_id combinations
eg. 
    0xc8 0x8c = "b*", this means the payload contains a dynamic amount of bytes specified by the "size" in the header
    0xc8 0x64 = "ds", this means the payload contains a "decimal" followed by a null terminated string

u = unsigned -> Type + 1 ie int = 0x14 uint = 0x15