#include "stdio.h"
#include "iostream"
#include <fstream>
#include <stdlib.h>
#include <map>
#include "string"
using namespace std;
/*
class CanonicalCode
{
    private:
        map<int,int> _code_bits_to_symbol ;
        map<int,int>::iterator it;
    public:
        CanonicalCode(int _codelengths[],int _length ){
            int _max = 0;
            for(int i=0;i < _length;i++){
                if(_codelengths[i] < 0)
                    printf("error from canonical Negative code length\n");
                if(_codelengths[i] > _max)
                    _max = _codelengths[i];
            }
            int nextcode = 0;
            for(int i=1;i < (_max+1);i++){
                nextcode <<= 1;
                int startbit = 1 << i;
                int enumerate = 0;
                for(int j=0;j<_length;j++){
                    if(_codelengths[j] != _codelengths[i])
                        continue;
                    if(nextcode >= startbit)
                        printf("This canonical code produces an over-full Huffman code tree");
                    _code_bits_to_symbol[startbit | nextcode] = j;
                    nextcode += 1;
                }
            }
            if (nextcode != 1 << _max)
                printf("This canonical code produces an under-full Huffman code tree");
        }

        int decode_next_symbol(int inp){
            int codebits = 1;
            while(true){
// inp should be bitsteam input => | inp.read_no_eof()
                codebits = codebits << 1 ;
                it = _code_bits_to_symbol.find(codebits);
                if (it!=_code_bits_to_symbol.end()){
                    int _result = _code_bits_to_symbol.find(codebits)->second;
                    return _result;
                }
            }
        }
};
/*
	def __str__(self):
		"""Returns a string representation of this canonical code,
		useful for debugging only, and the format is subject to change."""
		return "\n".join(
			"Code {}: Symbol {}".format(bin(codebits)[3 : ], symbol)
			for (codebits, symbol) in sorted(self._code_bits_to_symbol.items())) + "\n"
*/


class ByteHistory
{
    public:
//assume size always  32768
        int _size = 32768;
        int _data[32768] = { 0 };
        int _index;

        ByteHistory(){
            _index = 0;
        }

        void append(int b){
            if(0 <= _index && _index < _size){
                _data[_index] = b;
                _index = (_index+1)%_size;
            }
            else{
                printf("\ncannot append data in byte history \n");
            }
        }
//_out is int(?) >> OutputStream
        void _copy(int _dist,int _count,unsigned char _out){
            if(_count < 0 || !(1<=_dist && _dist <= _size)){
                printf("error from ByteHistory");
            }
            else {
                int readindex = (_index - _dist)%_size ;
                for(int i = 0;i<_count;i++){
                    int b = _data[readindex];
                    readindex = (readindex+1)%_size;
                //out.write(bytes((b,)) if python3 else chr(b)) => write(char(b))
                    printf("\nout.write from bytehistory : %02x\n",(unsigned char) b);
                    append(b);
                }
            }
        }

        int get_data(int i){
            return _data[i];
        }

};

class BitInputStream
{
     public:
        unsigned char *_inp;
        int _current_byte ;
        int _num_bits_remaining ;
        int _read_index;

        BitInputStream(unsigned char *inp){
            printf("\nBitinputstream here :");
            _inp = inp;
            _current_byte = 0;
            _num_bits_remaining = 0;
            _read_index = 0;
        }

        int get_bit_position(){
            if(0 <= _num_bits_remaining && _num_bits_remaining <=7)
                return ((-1*(_num_bits_remaining))%8)+8 ;
            else{
                printf("\nNum bit remaining not between 0-7\n");
                return -1;
            }
        }
        int read_byte(){
            _current_byte = 0;
            _num_bits_remaining = 0;
            _read_index += 1;
            return (int)_inp[_read_index-1];
        }
        int read(){
            if(_current_byte == (-1))
                return -1;
            if(_num_bits_remaining == 0){
                _current_byte  = (int)_inp[_read_index];
                _read_index += 1;
                if (_current_byte == -1)
                    return -1;
                _num_bits_remaining = 8;
            }
            if( _num_bits_remaining > 0){
                _num_bits_remaining -= 1;
                return ((_current_byte >> (7 - _num_bits_remaining)) & 1) ;
            }
            printf("\nAssertionError from read bitinputstream\n");
            return -1;
        }

        int read_no_eof(){
            int result;
            result =  read();
            if(result == -1){
                printf("\n error at read_no_eof \n");
                return -1;
            }
            return result;
        }
        void close(){
            _inp = 0x00;
            _current_byte = -1;
            _num_bits_remaining = 0;
        }
};


 void Decompressor(BitInputStream bitin,unsigned char out){
    printf("\nHello from decompressor :\n");
    BitInputStream _input = bitin;
    unsigned char _output = out;
    ByteHistory _dictionary = ByteHistory();
//Process the stream of blocks
    bool isfinal;
    do{
        isfinal = (_input.read_no_eof() == 1); //bfinal
        int _sum=0;
        for(int i=0;i<2;i++){
            _sum += _input.read_no_eof() << i;
            printf("sum is %d when i is %d ",_sum,i);
        }
        int type = _sum;
        printf("\n here is type value : %d\n",type);
        if(type == 0){

        }
        else if(type == 1){

        }
         else if(type == 2){

        }
         else if(type == 3){
            printf("\nReserved block type\n");
        }
         else{
            printf("\nImpossible value for type\n");
        }
    }while(!isfinal);
};

/* ****************************
 int d_read_int(int numbits){
    if(numbits < 0)
        printf("Error at read int \n");
    int _sum=0;
    for(int i=0;i<numbits;i++)
        _sum |= _input.read_no_eof() << i;
    return _sum;
}
*/
/*
class Decompressor
{
    public :
        BitInputStream _input;
        unsigned char _output;
 //*******       ByteHistory _dictionary = ByteHistory();
        int temp_array[288]= {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8};
        int temp_array2[32]= {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
 //********      CanonicalCode FIXED_LITERAL_LENGTH_CODE =  CanonicalCode(temp_array,288);
 //********       CanonicalCode FIXED_DISTANCE_CODE = CanonicalCode(temp_array2,32);


        /*unsigned char decompress_to_bytes (BitInputStream bitin){
            unsigned char out;
            decompress_to_stream(bitin,out);
            return out;
        }

        void decompress_to_stream(BitInputStream bitin,unsigned char out){
           // Decompressor(bitin, out);
        }

};
      /*  while(true){
            int isfinal =  0;//(bitin.read_no_eof() == 1)

        }*//*
        }
        /*
        void _decompress_uncompressed_block(){

        }
        *//*

        CanonicalCode decodeHuffmanCodes(){
              int numLitLenCodes = _read_int(5) +257;
              int numDistCodes = _read_int(5) + 1;
              int numCodeLenCodes = _read_int(4) + 4;
              int codeLenCodeLen[19];
              codeLenCodeLen[16] = _read_int(3);
              codeLenCodeLen[17] = _read_int(3);
              codeLenCodeLen[18] = _read_int(3);
              codeLenCodeLen[0] = _read_int(3);
              for(int i =0;i< numCodeLenCodes -4 ;i++){
                int j;
                if(i%2 ==0)
                    j = 8+i/2;
                else
                    j =  7-i/2;
                codeLenCodeLen[j] = _read_int(3);
              }
              CanonicalCode codeLenCode = CanonicalCode(codeLenCodeLen,19);
              int codeLens[numLitLenCodes + numDistCodes];
              for (int codeLensIndex = 0; codeLensIndex < numLitLenCodes + numDistCodes; ) {
                   int sym = codeLenCode.decode_next_symbol(_input);
                   if (0 <= sym && sym <= 15) {
                       codeLens[codeLensIndex] = sym;
                       codeLensIndex++;
                   } else {
                       int runLen;
                       int runVal = 0;
                       if (sym == 16) {
                           if (codeLensIndex == 0)
                                printf("Dataformat exception : No code length value to copy");
                           runLen = _read_int(2) + 3;
                           runVal = codeLens[codeLensIndex - 1];
                       } else if (sym == 17)
                           runLen = _read_int(3) + 3;
                       else if (sym == 18)
                           runLen = _read_int(7) + 11;
                       else
                           printf("AssertionError : Symbol out of range");
                       int _end = codeLensIndex + runLen;
                       if (_end > numLitLenCodes + numDistCodes)
                           printf("DataFormatException : Run exceeds number of codes");
                       for(int i=codeLensIndex ;i <_end ;i++)
                           codeLens[i] = runVal;
                       codeLensIndex = _end;
                   }
              }

              // Create literal-length code tree
              int litLenCodeLen[numLitLenCodes];
              for(int i=0;i<numLitLenCodes;i++){
                   litLenCodeLen[i] = codeLens[i];
              }
              CanonicalCode litLenCode = CanonicalCode(litLenCodeLen,numLitLenCodes);

              int distCodeLen[numLitLenCodes + numDistCodes  - numLitLenCodes];
              for(int i = 0 ; i<numLitLenCodes + numDistCodes - numLitLenCodes;i++){
                  if(numLitLenCodes+i > numLitLenCodes + numDistCodes)
                      distCodeLen[i] =  0;
                  else
                      distCodeLen[i] =  codeLens[numLitLenCodes+i];
              }
              bool check_distCode_null = false ;
              if ((numLitLenCodes + numDistCodes  - numLitLenCodes) == 1 && distCodeLen[0] == 0)
                  check_distCode_null = true;
              else{
                  int oneCount = 0;
                  int otherPositiveCount = 0;
                  for(int i=0;i<numLitLenCodes + numDistCodes  - numLitLenCodes;i++){
                        if(distCodeLen[i] == 1)
                              oneCount += 1;
                        else if(distCodeLen[i] > 1)
                              otherPositiveCount += 1;
                 }

                 if (oneCount == 1 && otherPositiveCount == 0) {
                        int  distCodeLen2[32];
                        for(int i =0 ;i<31;i++){
                              if(i < numLitLenCodes + numDistCodes  - numLitLenCodes)
                                  distCodeLen2[i] = distCodeLen[i];
                              else
                                  distCodeLen2[i] = 0;
                        }
                        distCodeLen2[31] = 1;
                 }
              }
              /*
              if(check_distCode_null)
                    CanonicalCode distCode = CanonicalCode([],0);
              else
                    CanonicalCode distCode = CanonicalCode(distCodeLen2,32);
              CanonicalCode[] re[2] = {litLenCode, distCode};
              return re;
              *//*
        }
        void _decompress_huffman_block(CanonicalCode litLenCode, CanonicalCode distCode){
            while(true){
                int sym = litLenCode.decode_next_symbol(_input);
                if(sym == 256)
                    break;
                if(sym < 256){
                   // _output.write(sym);
                    _dictionary.append(sym);
                } else {
                    int run = decode_run_length(sym);
                    if( 3 <= run || run <= 258)
                        printf("invalid run length");
                    //if(distCode == NULL)   how to check canonicalcode is null or not
                    //    printf("Length symbol encountered with empty distance code");
                    int distSym = distCode.decode_next_symbol(_input);
                    int dist = _decode_distance(distSym);
                    if(dist<1 || dist > 32768)
                        printf("invalid distance");
                    _dictionary._copy(dist, run,_output);
                }
            }
        };



        int decode_run_length(int sym){
            if( 257 <= sym && sym <= 287){
                if(sym <=264)
                    return sym-254;
                else if(sym <=284){
                    int numextrabits = (sym - 261) / 4;
                    return (((sym - 265) % 4 + 4) << numextrabits) + 3 + _read_int(numextrabits);
                }
                else if(sym == 285)
                    return 258;
                else
                    printf("Reserved length symbol: %d",sym);

            }
            else
                printf("Invalid run length symbol: %d",sym);
        }

        int _decode_distance(int sym){
            if( 0 <= sym && sym <= 31){
                if(sym <=3)
                    return sym+1;
                else if(sym <=29){
                    int numextrabits = sym / 2 - 1 ;
                    return ((sym % 2 + 2) << numextrabits) + 1 + _read_int(numextrabits);
                }
                else
                    printf("Reserved distance symbol: %d",sym);
            }
            else
                printf("Invalid distance symbol: %d",sym);
        }


 //   public :
/*
    @staticmethod
	def decompress_to_bytes(bitin):
		"""Reads from the given input stream, decompress the data, and returns a new byte list."""
		out = io.BytesIO() if python3 else StringIO.StringIO()
		Decompressor.decompress_to_stream(bitin, out)
		return out.getvalue()


	@staticmethod
	def decompress_to_stream(bitin, out):
		"""Reads from the given input stream, decompress the data, and writes to the given output stream."""
		Decompressor(bitin, out)


};
*/
int main(int argc, char**argv)
{
    FILE *fileptr;
    unsigned char *buffer;
    unsigned char compmeth,flag,extraflags,osbyte;
    long filelen;
    char infile[] = "input.gz";
    char outfile[] = "output";
    int read_index=0; //for inp.read(1) in py
    fileptr = fopen(infile, "rb");  // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file

    buffer = (unsigned char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file
    for(int i=0;i<filelen;i++)  {
        printf("%02x ",(unsigned char)buffer[i]);
    }
// Header
    if((unsigned char)buffer[read_index] != 0x1f){
        printf("\nInvalid GZIP magic number");
        return 0;
    }
    read_index += 1;
     if((unsigned char)buffer[read_index] != 0x8b){
        printf("\nInvalid GZIP magic number");
        return 0;
    }
    read_index += 1;

    compmeth = (unsigned char)buffer[read_index];
    read_index +=1;
    printf("\ncheckpoint 3");
     if(compmeth != 0x08){
        printf("\nUnsupported compression method");
        return 0;
    }
// Reserved flags
    flag = (unsigned char)buffer[read_index];
    read_index += 1;
    if((flag & 0xe0) != 0){
        printf("\nReserved flags are set");
        return 0;
    }
// Modification time
    printf("\ncheckpoint 4");
    int mtime;
    int temp16;
    temp16 = ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8);
    read_index +=2;
    //printf("\n%d",temp16);
    mtime = temp16 | ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8) << 16;
    read_index +=2;
    //printf("\n%d",mtime);
    if(mtime != 0){
        printf("\nLast modified: (1970, 1, 1) + %d seconds",mtime);
    }
    else
    {
        printf("\nLast modified: N/A");
    }

//Extra flag
    extraflags = (unsigned char)buffer[read_index];
    read_index +=1;
//    printf("%02x",extraflags);
    if(extraflags == 0x02){
        printf("\nExtra flags: Maximum compression");
    }
    else if(extraflags == 0x04){
        printf("\nExtra flags: Fastest compression");
    }
    else {
        printf("\nExtra flags: Unknown ");
    }

// Operating system
    osbyte = (unsigned char)buffer[read_index];
    read_index +=1;
    switch((unsigned int)osbyte){
        case 0: printf("\nOperating system: FAT");break;
        case 1: printf("\nOperating system: Amiga");break;
        case 2: printf("\nOperating system: VMS");break;
        case 3: printf("\nOperating system: Unix");break;
        case 4: printf("\nOperating system: VM/CMS");break;
        case 5: printf("\nOperating system: Atari TOS");break;
        case 6: printf("\nOperating system: HPFS");break;
        case 7: printf("\nOperating system: Macintosh");break;
        case 8: printf("\nOperating system: Z-System");break;
        case 9: printf("\nOperating system: CP/M");break;
        case 10: printf("\nOperating system: TOPS-20");break;
        case 11: printf("\nOperating system: NTFS");break;
        case 12: printf("\nOperating system: QDOS");break;
        case 13: printf("\nOperating system: Acorn RISCOS");break;
        case 255: printf("\nOperating system: Unknown");break;
        default : printf("Operating system: Really unknown");break;
    }

//Handle assorted flags
    if(flag & 0x01 !=0 )
        printf("\nFlag: Text");
    if(flag & 0x04 !=0 ){
        printf("\nFlag: Extra");
        int countt;
        countt = ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8);
        read_index +=2;
        while(countt > 0){ //skip extra data
            break; //can't test yet
        }
    }
    printf("\n%d",flag & 0x08);
    if((flag & 0x08) != 0 ){
        string file_name = "";
        while(true){
            file_name += (unsigned char)buffer[read_index];
            printf("\n%02c",(unsigned char)buffer[read_index]);
            if((unsigned int)buffer[read_index] == 0x00)
                break;
            read_index +=1;
        }
        cout << "File name: " << file_name << '\n';
        read_index +=1;
    }
    if(flag & 0x02 != 0 ){
        printf("Header CRC-16: %04x",((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8));
        read_index +=2;
    }
    if(flag & 0x10 != 0){
        string file_name = "";
        while(true){
            file_name += (unsigned char)buffer[read_index];
            printf("\n%02c",(unsigned char)buffer[read_index]);
            if((unsigned int)buffer[read_index] == 0)
                break;
            read_index +=1;
        }
        cout << "Comment: " << file_name << '\n';
        read_index +=1;
    }

//Decompress
    try{
        unsigned char *bdecom , out;
        bdecom = (unsigned char *)malloc((filelen+1-read_index)*sizeof(char));
        for(int i=0;i<filelen+1-read_index;i++)
            bdecom[i] = buffer[read_index+i];
        for(int i=0;i<filelen+1-read_index;i++)  {
            printf("%02x ",(unsigned char)bdecom[i]);
        }
        cout << "yea" ;
        BitInputStream bitin = BitInputStream(bdecom);
        Decompressor(bitin,out);
       // unsigned char decomp = d.decompress_to_bytes(bitin);
/*try:
				bitin = deflatedecompress.BitInputStream(inp)
				decomp = deflatedecompress.Decompressor.decompress_to_bytes(bitin)
*/
    }
    catch (exception& e){
        cout <<"Standard exception: " <<  e.what() << '\n';
    }


    return 0;
}
