#!/bin/env python
# Samuel Jero <sjero@purdue.edu>
# Based upon work by Hyo Lee
# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
import os
import sys
import argparse
import re
from types import NoneType


class MessageFormatParser:
    def __init__(self):
        self.structs = dict()
        self.pkts = dict()
        self.tags = []
        self.type_field = None
        self.max_fields = 0

    def addFormatLines(self, lines):
        struct = None
        ptype = None

        #Parse Format
        for l in lines:
            if len(l) <= 1 or l[0] == "#" or l[0] == "\n":
                continue
            if l.find("//") >= 0:
                continue
            if struct is not None:
                if l.find("}") >=0:
                    if 'name' not in struct:
                        print "Warning: struct with no name %s" % (struct)
                    self.structs[struct['name']] = struct
                    struct = None
                else:
                    f = self._find_field(l)
                    if f is not None:
                        struct['fields'].append(f)
            elif ptype is not None:
                if l.find("}") >=0:
                    if 'name' not in ptype:
                        print "Warning: packet with no name %s" % (ptype)
                    self.pkts[ptype['name']] = ptype
                    ptype = None
                else:
                    f = self._find_field(l)
                    if f is not None:
                        ptype['fields'].append(f)
            elif l.find("DEFINE") >= 0:
                #define
                parts = l.split()
                if len(parts) != 3:
                    print "Warning: unexpected DEFINE: %s" % (l.strip())
                    continue
                if parts[1] == "TYPE_FIELD":
                    self.type_field = parts[2]
                d = {'name':parts[1], 'value':parts[2]}
                self.tags.append(d)
            elif l.find("struct") >= 0:
                #Struct
                parts = l.split()
                if len(parts) != 3:
                    print "Warning: invalid Struct: %s" % (l.strip())
                name = parts[1]
                struct = {'name':name,'fields':[]}
            elif l.find("{") >= 0:
                #Type
                parts = l.split()
                if len(parts) != 2:
                    print "Warning: invalid Type: %s" % (l.strip())
                name = parts[0]
                ptype = {'name':name,'fields':[]}
            else:
                print "Error: Unknown line \"%s\"" % (l.strip())

        #Check Packet types
        if self.type_field is None:
            print "Warning: No type field indicated!"
        else:
            for k in self.pkts:
                p = self.pkts[k]
                found = False
                for f in p['fields']:
                    if f['length'] == "BaseMessage":
                        p['encap'] = True
                    if f['name'] == self.type_field:
                        if 'value' in f:
                            p['type'] = f['value']
                            if found:
                                print "Warning: Multiple type fields in packet %s. Using most recent." % (p['name'])
                            found = True
                if not found:
                    print "Warning: No type field in packet %s" % (p['name'])

                        

    def _find_field(self, l):
        parts = l.split()
        if len(parts) < 2 or len(parts) > 4:
            print "Warning: unexpcted field: %s" % (l.strip())
            return None
        struct = None
        if parts[0].find("struct") >= 0:
            struct = True
            #Remove the extra word
            del parts[0]
        ftype = parts[0]
        fname = parts[1].replace(";","")
        bitfield = None
        index = None
        value = None
        variable = None
        if fname.find(":") >= 0:
            p = fname.split(":")
            fname = p[0]
            bitfield = ":" + p[1]
        if fname.find("[") >= 0:
            p = fname.split("[")
            fname = p[0]
            index = p[1].replace("[","").replace("]","")
            try:
                i = int(index)
            except Exception as e:
                print "Warning: variable length fields not supported: \"%s\"" % (l.strip())
                variable = True
        if len(parts) == 3:
            print "Warning: invalid packet format in line \"%s\"" % (l.strip())
            return None
        if len(parts) == 4:
            if parts[2] == "=":
                value = parts[3].replace(";","")
            else:
                print "Warning: invalid packet format in line \"%s\"" % (l.strip())
                return None
        f = {'name':fname,'length':ftype}
        if value is not None:
            f['value']=value
        if index is not None:
            f['index'] = index
        if bitfield is not None:
            f['bitfield'] = bitfield
        if variable is not None:
            f['variable'] = True
        if struct is not None:
            f['struct'] = True
        return f

    def outputHeader(self,writer):
        header = """#include "ns3/uinteger.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <ns3/ipv4-address.h>
#ifndef MESSAGE_H
#define MESSAGE_H
"""
        writer.write(header)
        for t in self.tags:
            writer.write("#define %s %s\n" % (t['name'],t['value']))
        writer.write("\n")

        for k in self.structs:
            s = self.structs[k]
            writer.write("struct %s {\n" % (s['name']))
            for f in s['fields']:
                self._print_field(f,writer,0)
            writer.write("};\n\n")
                

        for k in self.pkts:
            p = self.pkts[k]
            writer.write("typedef struct {\n")
            i = 0
            for f in p['fields']:
                i = self._print_field(f,writer,i)
            if i > self.max_fields:
                self.max_fields = i
            writer.write("} %s;\n\n" % (p['name']))

        writer.write("enum MessageType {\n")
        if self.type_field is not None:
            for k in self.pkts:
                p = self.pkts[k]
                if 'type' in p:
                    writer.write("%s=%s,\n" % (k.upper(),p['type']))
        else:
            i = 0
            for k in self.pkts:
                p = self.pkts[k]
                writer.write("%s=%s,\n" % (k.upper(),i))
                i += 1
        writer.write("};\n\n")

        header = """class Message {
    public:

    uint8_t *msg;
    int type;
    int size;

    Message *encMsg;

    Message(uint8_t* msg);

    int FindMsgType();
    int FindMsgSize();
    uint8_t * EncMsgOffset();
    static int StrToType(const char* str);
    static std::string TypeToStr(int type);
    void ChangeValue(int field, char* value);
"""
        writer.write(header)
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("\tvoid Change%s(int field, char* value);\n" %(p['name']))
        writer.write("\tvoid CreateMessage(int type, const char *spec);\n")
        writer.write("\tstatic int GetMessageHeaderSize(int type);\n")
        header = """
    uint16_t GetSourcePort();
    uint16_t GetDestPort();
    uint32_t GetSequenceNumber();
    uint32_t GetAcknowledgementNumber();
    void SetSequenceNumber(uint32_t seq);
    void SetAcknowledgementNumber(uint32_t ack);
    void DoChecksum(int len, ns3::Ipv4Address src, ns3::Ipv4Address dest, int proto);

    private:
    uint32_t checksum(u_char *buf, unsigned nbytes, uint32_t sum);
    uint32_t wrapsum(uint32_t sum);
    };
#endif

"""
        writer.write(header)
        writer.write("#define MSG %s\n" % (len(self.pkts)))
        writer.write("#define FIELD %s\n" % (str(self.max_fields)))

                

    def outputBody(self,writer):
        header = """#include "message.h"

Message::Message(uint8_t* m) {
    //std::cout << "Entering Message" << std::endl;
    msg = m;
    type = FindMsgType();
    size = FindMsgSize();

    encMsg = NULL;
    uint8_t *ptr = EncMsgOffset();
    if (ptr != NULL) {
        encMsg = new Message(ptr);
    }
    //std::cout << "Exiting Message" << std::endl;
}

int Message::FindMsgType() {
#ifdef TYPE_FIELD
    return ((BaseMessage*)msg)->TYPE_FIELD;
#else
    return 0;
#endif
}

int Message::FindMsgSize() {
#ifdef SIZE_FIELD
#ifdef SIZE_MULT
    return ((BaseMessage*)msg)->SIZE_FIELD*SIZE_MULT;
#else
    return ((BaseMessage*)msg)->SIZE_FIELD;
#endif
#else
    return sizeof(BaseMessage);
#endif
}
"""
        writer.write(header)
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("void Message::Change%s(int field, char* value) {\n" % (p['name']))
            writer.write("\t//std::cout << \"Entering Change%s\" << std::endl;\n"%(p['name']))
            writer.write("\t%s *ptr = (%s*)msg;\n\n" % (p['name'],p['name']))
            i = 0
            for f in p['fields']:
                i = self._print_field_processing(f,writer,i)

            writer.write("\t//std::cout << \"Exiting Change%s\" << std::endl;\n" % (p['name']))
            writer.write("}\n\n")

        header = """void Message::ChangeValue(int field, char* value) {
        //std::cout << "Entering ChangeValue " << type << " " << field << " " << value << std::endl;
        switch (type) {
"""
        writer.write(header)
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("\t\tcase %s:\n" % (p['name'].upper()))
            writer.write("\t\t\tChange%s(field,value);\n" % (p['name']))
            writer.write("\t\t\tbreak;\n")
        writer.write("\t}\n")
        writer.write("\t//std::cout << \"Exiting ChangeValue\" << std::endl;\n")
        writer.write("}\n\n")

        writer.write("int Message::StrToType(const char *str) {\n")
        writer.write("\t//std::cout << \"Entering StrToType\" << std::endl;\n")
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("\tif (strcmp(str,\"%s\") == 0) {\n" % (p['name']))
            writer.write("\t\treturn %s;\n" % (p['name'].upper()))
            writer.write("\t}\n")
        writer.write("\t//std::cout << \"Exiting StrToType\" << std::endl;\n")
        writer.write("\treturn -1;\n")
        writer.write("}\n\n")

        writer.write("std::string Message::TypeToStr(int type){\n")
        writer.write("\t//std::cout << \"Entering TypeToStr\" << std::endl;\n")
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("\tif( type == %s) {\n" % (p['name'].upper()))
            writer.write("\t\treturn \"%s\";\n" % (p['name']))
            writer.write("\t}\n")
        writer.write("\t//std::cout << \"Exiting TypeToStr\" << std::endl;\n")
        writer.write("\treturn \"Invalid\";\n")
        writer.write("}\n\n")

        writer.write("uint8_t* Message::EncMsgOffset() {\n")
        writer.write("\t//std::cout << \"Entering EncMsgOffset\" << std::endl;\n")
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("\tif( type == %s) {\n" % (p['name'].upper()))
            if 'encap' in p:
                writer.write("\t\treturn (uint8_t*) msg + sizeof(%s)" % (p['name']))
            else:
                writer.write("\t\treturn NULL;\n")
                writer.write("\t}\n")
        writer.write("\treturn NULL;\n")
        writer.write("\t//std::cout << \"Exiting EncMsgOffset\" << std::endl;\n")
        writer.write("}\n")

        header = """
void Message::CreateMessage(int type, const char *spec){
        //std::cout<< "Entering CreateMessage"<<std::endl;
        int field;
        char fspec[1000];
        int len, ret;

#ifdef TYPE_FIELD
        this->type=type;
        ((BaseMessage*)msg)->TYPE_FIELD=type;
#endif

        fspec[0]='=';
        ret=sscanf(spec, "%i=%999s%n",&field,&fspec[1],&len);
        spec+=len;
        while(ret!=EOF){
                ChangeValue(field,fspec);
                ret=sscanf(spec, "%i=%999s%n",&field,&fspec[1],&len);
                spec+=len;
        };
        //std::cout<< "Exiting CreateMessage"<<std::endl;
}

"""
        writer.write(header)
        
        writer.write("int Message::GetMessageHeaderSize(int type){\n")
        writer.write("\t//std::cout<< \"Entering GetMessageHeaderSize\"<<std::endl;\n")
        writer.write("\tswitch(type) {\n")
        for k in self.pkts:
            p = self.pkts[k]
            writer.write("\tcase %s:\n" % (p['name'].upper()))
            writer.write("\t\treturn sizeof(%s);\n" % (p['name']))
        writer.write("\tdefault:\n")
        writer.write("\t\treturn 0;\n")
        writer.write("\t}\n")
        writer.write("\t//std::cout<< \"Exiting GetMessageHeaderSize\"<<std::endl;\n")
        writer.write("}\n\n")

        header = """uint16_t Message::GetSourcePort()
{
#ifdef SOURCE_PORT_FIELD
    return ntohs(((BaseMessage*)msg)->SOURCE_PORT_FIELD);
#else
    return 0;
#endif
}

uint16_t Message::GetDestPort()
{
#ifdef DEST_PORT_FIELD
    return ntohs(((BaseMessage*)msg)->DEST_PORT_FIELD);
#else
    return 0;
#endif
}

uint32_t Message::GetSequenceNumber()
{
#ifdef SEQUENCE_FIELD
    return ntohl(((BaseMessage*)msg)->SEQUENCE_FIELD);
#else
    return 0;
#endif
}

uint32_t Message::GetAcknowledgementNumber(){
#ifdef ACKNOWLEDGEMENT_FIELD
    return ntohl(((BaseMessage*)msg)->ACKNOWLEDGEMENT_FIELD);
#else
    return 0;
#endif
}

void Message::SetSequenceNumber(uint32_t seq)
{
#ifdef SEQUENCE_FIELD
    ((BaseMessage*)msg)->SEQUENCE_FIELD=htonl(seq);
#endif
}

void Message::SetAcknowledgementNumber(uint32_t ack)
{
#ifdef ACKNOWLEDGEMENT_FIELD
    ((BaseMessage*)msg)->ACKNOWLEDGEMENT_FIELD=htonl(ack);
#endif
}

/*Checksumming Code pulled from dccp2tcp (https://github.com/samueljero/dccp2tcp) by Samuel Jero
 * --- Code is under GNU GPL */
struct ip4_pseudo_hdr{
    uint32_t    src;
    uint32_t    dest;
    uint32_t    len;
    char        zero[3];
    char        nxt;
};

/*From http://gitorious.org/freebsd/freebsd/blobs/HEAD/sbin/dhclient/packet.c
 * under GNU GPL*/
uint32_t Message::checksum(u_char *buf, unsigned nbytes, uint32_t sum)
{
    int i;
    /* Checksum all the pairs of bytes first... */
    for (i = 0; i < (nbytes & ~1U); i += 2) {
        sum += (u_int16_t)ntohs(*((u_int16_t *)(buf + i)));
        if (sum > 0xFFFF)
            sum -= 0xFFFF;
    }
    /*
     * If there's a single byte left over, checksum it, too.
     * Network byte order is big-endian, so the remaining byte is
     * the high byte.
     */
    if (i < nbytes) {
        sum += buf[i] << 8;
        if (sum > 0xFFFF)
            sum -= 0xFFFF;
    }
    return (sum);
}

/*From http://gitorious.org/freebsd/freebsd/blobs/HEAD/sbin/dhclient/packet.c
 * under GNU GPL*/
uint32_t Message::wrapsum(uint32_t sum)
{
    sum = ~sum & 0xFFFF;
    return (htons(sum));
}

void Message::DoChecksum(int len, ns3::Ipv4Address src, ns3::Ipv4Address dest, int proto){
    struct ip4_pseudo_hdr hdr;

#ifdef CHECKSUM_FIELD
    //create pseudo header
    memset(&hdr, 0, sizeof(struct ip4_pseudo_hdr));
    hdr.src=htonl(src.Get());
    hdr.dest=htonl(dest.Get());
    hdr.nxt=proto;
    hdr.len=htonl(len);

    //calculate total checksum
    ((BaseMessage*)msg)->CHECKSUM_FIELD=0;
    ((BaseMessage*)msg)->CHECKSUM_FIELD=wrapsum(checksum((u_char*)&hdr,sizeof(struct ip4_pseudo_hdr),checksum(msg,len,0)));
#endif
}
"""
        writer.write(header)



    def _print_field_processing(self,field,writer,num):
        if 'variable' in field:
            #TODO
            print "Warning: Variable length fields are not supported: %s" % (str(field))
            return num
        if 'struct' in field:
            try:
                s = self.structs[field['length']]
            except Exception as e:
                return num
            if s is None:
                return num
            for f in s['fields']:
                num = self._print_field_processing(f,writer,num)
            return num
        writer.write("\tif (field == %d) {\n" % (num))
        if 'index' in field and field['length'] != "char":
            #TODO
            print "Warning: non-characer arrays are not supported: %s" % (str(field))
        else:
            extract = "ptr->%s" % (field['name'])
            self._print_field_changes(field,extract,writer)
        writer.write("\t}\n\n")
        num += 1
        return num

    def _print_field_changes(self,field,extract,writer):
        le_types = ['int8_t', 'uint8_t', 'int64_t','int32_t','int16_t']
        if field['length'] == "char":
            if 'index' in field:
                i = int(field['index'])
                writer.write("\t\tstrncpy(%s,value,%d);\n" % (extract,i))
            else:
                writer.write("\t\t%s = *value;" % (extract));
        elif field['length'] == "BaseMessage":
            #Encapsulated message, ignore
            pass
        elif field['length'] == "double" or field['length'] == "float":
            writer.write("\t\t%s *cur = &%s;\n" % (field['length'],extract))
            writer.write("\t\tif(value[0] == '=') {\n")
            writer.write("\t\t\t*cur = (%s)atof(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '+') {\n")
            writer.write("\t\t\t*cur += (%s)atof(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '-') {\n")
            writer.write("\t\t\t*cur -= (%s)atof(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '*') {\n")
            writer.write("\t\t\t*cur *= (%s)atof(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '/') {\n")
            writer.write("\t\t\t*cur /= (%s)atof(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == 'r') {\n")
            writer.write("\t\t\t*cur = (%s) rand();\n" % (field['length']))
            writer.write("\t\t}\n")
        elif field['length'] == "uint16_t" and 'bitfield' not in field:
            writer.write("\t\t%s *cur = &%s;\n" % (field['length'],extract))
            writer.write("\t\t%s tmp = ntohs(*cur);\n" % (field['length']))
            writer.write("\t\tif(value[0] == '=') {\n")
            writer.write("\t\t\ttmp = (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '+') {\n")
            writer.write("\t\t\ttmp += (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '-') {\n")
            writer.write("\t\t\ttmp -= (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '*') {\n")
            writer.write("\t\t\ttmp *= (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '/') {\n")
            writer.write("\t\t\ttmp /= (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == 'r') {\n")
            writer.write("\t\t\ttmp = (%s) rand();\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\t*cur = htons(tmp);\n")
        elif field['length'] == "uint32_t" and 'bitfield' not in field:
            writer.write("\t\t%s *cur = &%s;\n" % (field['length'],extract))
            writer.write("\t\t%s tmp = ntohl(*cur);\n" % (field['length']))
            writer.write("\t\tif(value[0] == '=') {\n")
            writer.write("\t\t\ttmp = (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '+') {\n")
            writer.write("\t\t\ttmp += (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '-') {\n")
            writer.write("\t\t\ttmp -= (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '*') {\n")
            writer.write("\t\t\ttmp *= (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '/') {\n")
            writer.write("\t\t\ttmp /= (%s)atoi(value+1);\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == 'r') {\n")
            writer.write("\t\t\ttmp = (%s) rand();\n" % (field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\t*cur = htonl(tmp);\n")
        elif field['length'] in le_types or 'bitfield' in field:
            writer.write("\t\tif(value[0] == '=') {\n")
            writer.write("\t\t\t%s = (%s)atoi(value+1);\n" % (extract,field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '+') {\n")
            writer.write("\t\t\t%s += (%s)atoi(value+1);\n" % (extract,field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '-') {\n")
            writer.write("\t\t\t%s -= (%s)atoi(value+1);\n" % (extract,field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '*') {\n")
            writer.write("\t\t\t%s *= (%s)atoi(value+1);\n" % (extract,field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == '/') {\n")
            writer.write("\t\t\t%s /= (%s)atoi(value+1);\n" % (extract,field['length']))
            writer.write("\t\t}\n")
            writer.write("\t\tif(value[0] == 'r') {\n")
            writer.write("\t\t\t%s = (%s) rand();\n" % (extract,field['length']))
            writer.write("\t\t}\n")
        else:
            print "Warning: Unknown field length: %s " % (str(field))
        
        

    def _print_field(self, field,writer, num): 
        post = ""
        if 'variable' in field:
            return num
        if field['length'] == "BaseMessage":
            return num
        if 'bitfield' in field:
            post = field['bitfield']
        if 'index' in field:
            post = "[" + field['index'] + "]"
        if 'struct' in field:
            #Struct field
            try:
                s = self.structs[field['length']]
            except Exception as e:
                return num
            if s is None:
                return num
            for f in s['fields']:
                num = self._print_field(f,writer, num)
            return num
        else:
            #Normal field
            writer.write("\t%s %s%s;\n" % (field['length'],field['name'],post))
            num += 1
            return num

    def outputStrategies(self, writer):
        writer.write("BaseMessage NONE 0\n")
        writer.write("BaseMessage NONE 0\n")

        for k in self.pkts:
            p = self.pkts[k]
            writer.write("%s DROP 100\n" % (p['name']))
            writer.write("%s DUP 2\n" % (p['name']))
            writer.write("%s DELAY 1.0\n" % (p['name']))
            writer.write("%s DIVERT 0\n" % (p['name']))
            writer.write("%s REPLAY 0 %s DIVERT 0\n" % (p['name'],p['name']))
            i = 0
            for f in p['fields']:
                i = self._print_field_strats(f,p,writer,i)

    def _print_field_strats(self,field,pkt,writer,num):
        if 'variable' in field:
            return num
        if field['length'] == "BaseMessage":
            return num
        if 'struct' in field:
            #Struct field
            try:
                s = self.structs[field['length']]
            except Exception as e:
                return
            if s is None:
                return
            for f in s['fields']:
                num = self._print_field_strats(f,pkt,writer,num)
            return num
        else:
            #Normal field
            if 'bitfield' in field:
                l = 0
                try:
                    l = int(field['bitfield'].replace(":",""))
                except Exception as e:
                    print "Warning: Bad bitfield: %s" % (str(field))
                    return num + 1
                if l == 1:
                    writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =1 %d\n" % (pkt['name'],num))
                elif l == 2:
                    writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =1 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =2 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =3 %d\n" % (pkt['name'],num))
                elif l == 4:
                    writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =2 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =3 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =4 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =8 %d\n" % (pkt['name'],num))
                elif l == 5:
                    writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =2 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =4 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =8 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =16 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =31 %d\n" % (pkt['name'],num))
                elif l == 6:
                    writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =2 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =4 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =10 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =30 %d\n" % (pkt['name'],num))
                    writer.write("%s LIE =63 %d\n" % (pkt['name'],num))
                else:
                    print "Warning: Not generating stratgies for field \"%s\": bitfield length unknown" % (field['name'])
            elif field['length'] == "int8_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-128 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =128 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-32 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =32 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-16 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =16 %d\n" % (pkt['name'],num))
            elif field['length'] == "uint8_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =255 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =128 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =64 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =32 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =16 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =8 %d\n" % (pkt['name'],num))
            elif field['length'] == "int16_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-32765 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =32765 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-1024 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1024 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-128\n" % (pkt['name'],num))
                writer.write("%s LIE =128 %d\n" % (pkt['name'],num))
            elif field['length'] == "uint16_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =65535 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =16384 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =4096 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1024 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =256 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =64 %d\n" % (pkt['name'],num))
            elif field['length'] == "int32_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-2147483647 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =2147483647 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-4096 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =4096 %d\n" % (pkt['name'],num))
            elif field['length'] == "uint32_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =4294967295 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =131072 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =4096 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =128 %d\n" % (pkt['name'],num))
            elif field['length'] == "int64_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-9223372036854775807LL %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-17179869184 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =16777216 %d\n" % (pkt['name'],num))
            elif field['length'] == "uint64_t":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =134217728 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =9223372036854775807LL %d\n" % (pkt['name'],num))
                writer.write("%s LIE =17179869184 %d\n" % (pkt['name'],num))
            elif field['length'] == "double":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-1E+37 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1E+37 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1000000000 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-1000000000 %d\n" % (pkt['name'],num))
            elif field['length'] == "float":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-1E+37 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1E+37 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1000000000 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =-1000000000 %d\n" % (pkt['name'],num))
            elif field['length'] == "bool":
                writer.write("%s LIE r %d\n" % (pkt['name'],num))
                writer.write("%s LIE =0 %d\n" % (pkt['name'],num))
                writer.write("%s LIE =1 %d\n" % (pkt['name'],num))
            else:
                print "Warning: Not generating strategies for field \"%s\": unknown type\n" % (field['name'])
        num +=1
        return num
            

def main(args):
    #Parse args
    argp = argparse.ArgumentParser(description='Parse Message format')
    argp.add_argument('format_file', help="Input: Format File")
    args = vars(argp.parse_args(args[1:]))

    try:
        format_file = open(args['format_file'],"r")
    except Exception as e:
        print "Failed to Open Format File %s:%s" % (args['format_file'],e)
        return

    try:
        header_file = open("message.h","w")
    except Exception as e:
        print "Failed to Open Header File %s:%s" % ("message.h",e)
        return
    try:
        cc_file = open("message.cc","w")
    except Exception as e:
        print "Failed to Open CC File %s:%s" % ("message.cc",e)
        return
    try:
        strategy_file = open("strategy","w")
    except Exception as e:
        print "Failed to Open Strategy File %s:%s" % ("strategy",e)
        return

    lines = format_file.readlines()
    format_file.close()

    hfp = MessageFormatParser()
    hfp.addFormatLines(lines)
    hfp.outputHeader(header_file)
    header_file.close()
    hfp.outputBody(cc_file)
    cc_file.close()
    hfp.outputStrategies(strategy_file)
    strategy_file.close()


if __name__ == "__main__":
    main(sys.argv)