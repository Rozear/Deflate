#include "stdio.h"
#include "iostream"
#include <fstream>
#include <stdlib.h>
#include "string"
using namespace std;
int read_index=0; //for inp.read(1) in py

//allocate memory
class CanonicalCode
{
    public:
        int MAX_CODE_LENGTH = 15;
        int symbolCodeBits[1000] ;
        int symbolValues[1000] ;
        int numSymbolsAllocated =0;
        CanonicalCode(int *codeLengths,int len){
            int _max = 0;
            int codeLengths_length=0;
            for(int i=0;i<len;i++){
                if(codeLengths[i] < 0)
                    printf("error from canonical Negative code length\n");
                if(codeLengths[i] > _max)
                    _max = codeLengths[i];
                codeLengths_length += 1;
            }
            int symbolCodeBits2[codeLengths_length];
            int symbolValues2[codeLengths_length];
            int nextCode = 0;
            for(int i=1;i < (_max+1);i++){
                nextCode <<= 1;
                int startbit = 1 << i;
                int enumerate = 0;
                for(int symbol =0;symbol <codeLengths_length;symbol ++){
                    if(codeLengths[symbol] != i)
                        continue;
                    if(nextCode >= startbit)
                        printf("This canonical code produces an over-full Huffman code tree");
                    symbolCodeBits2[numSymbolsAllocated] = startbit | nextCode;
                    symbolValues2[numSymbolsAllocated] = symbol;
                    numSymbolsAllocated +=1;
                    nextCode += 1;
                }
            }
            for(int i =0 ;i< numSymbolsAllocated ;i++){
                symbolCodeBits[i] = symbolCodeBits2[i];
                symbolValues[i] = symbolValues2[i];
            }

            if (nextCode != 1 << _max)
                printf("This canonical code produces an under-full Huffman code tree");
        }
};


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
        void _copy(int _dist,int _count,unsigned char * _out,int oindex){
            if(_count < 0 || !(1<=_dist && _dist <= _size)){
                printf("error from ByteHistory");
            }
            else {
                int readindex = (_index - _dist)%_size ;
                for(int i = 0;i<_count;i++){
                    int b = _data[readindex];
                    readindex = (readindex+1)%_size;
                //out.write(bytes((b,)) if python3 else chr(b)) => write(char(b))
                    _out[oindex] = (unsigned char)b ;
                    oindex += 1;
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
            read_index +=1;  //*******************************************************************//
            return (int)_inp[_read_index-1];
        }
        int read(){
            if(_current_byte == (-1))
                return -1;
            if(_num_bits_remaining == 0){
                _current_byte  = (int)_inp[_read_index];
                _read_index += 1;
                read_index +=1;  //*******************************************************************//
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


//canonical function ~~~~
 int decode_next_symbol(BitInputStream  &inp,CanonicalCode c){
     int codeBits =1;
     int index = -1;
     int reof =0;

     while(true){
        reof = inp.read_no_eof();
        codeBits = codeBits << 1 | reof;
        printf("\ncodebits is : %d",codeBits);
        for(int i=0;i<c.numSymbolsAllocated;i++){
                if(codeBits == c.symbolCodeBits[i]){
                    printf("\ndecode_next_symbol: %d %d",codeBits,c.symbolValues[i]);
                    index = i;
                }
        }
        if (index >= 0)
				return c.symbolValues[index];

     }
}

CanonicalCode * _decode_huffman_codes(BitInputStream &_input){

    int _sum=0;
    for(int i=0;i<5;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    int numLitLenCodes = _sum + 257;
    _sum=0;

    for(int i=0;i<5;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    int numDistCodes = _sum + 1;
    _sum=0;

    for(int i=0;i<4;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    int numCodeLenCodes = _sum + 4;
    _sum=0;

    int codeLenCodeLen[19];
    for(int i=0;i<19;i++)
        codeLenCodeLen[i]=0;
    for(int i=0;i<3;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    codeLenCodeLen[16] = _sum;
    _sum=0;
    for(int i=0;i<3;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    codeLenCodeLen[17] = _sum;
    _sum=0;
    for(int i=0;i<3;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    codeLenCodeLen[18] = _sum;
    _sum=0;
    for(int i=0;i<3;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    codeLenCodeLen[0] = _sum;
    _sum=0;
    for (int i = 0; i < numCodeLenCodes - 4; i++) {
        int _summ =0;
        int j = 0;
        if(i%2 == 0)
            j = (8 + i / 2);
        else
            j = (7 - i / 2);
        for(int k=0;k<3;k++){
        _summ += _input.read_no_eof() << k;
        //  printf("sum is %d when i is %d ",_sum,i);
        }
        codeLenCodeLen[j] =  _summ;
    }
    printf("\nnext check point\n");
    CanonicalCode codeLenCode = CanonicalCode(codeLenCodeLen,19);
/*
    printf("\nhi\n");
    for(int i =0;i<codeLenCode.numSymbolsAllocated;i++)
        printf("%d ",codeLenCode.symbolCodeBits[i]);
*/
    int codeLens[numLitLenCodes+numDistCodes];
    for(int i=0;i<numLitLenCodes+numDistCodes;i++)
        codeLens[i]=0;
    printf("\n....  %d",numLitLenCodes);

    for (int codeLensIndex = 0; codeLensIndex < numLitLenCodes+numDistCodes; ) {
			int sym = decode_next_symbol(_input,codeLenCode);
         /*   printf("\n");
			for(int i=0;i<40;i++)
                cout << _input.read_no_eof();
            printf("\n");*/
			printf("\nsym = %d\n",sym);
			if (0 <= sym && sym <= 15) {
			    codeLens[codeLensIndex] = sym;
				codeLensIndex++;
			}
			else {
                int runLen;
                int runVal =0;
                if (sym == 16) {
					if (codeLensIndex == 0)
						printf("\nNo code length value to copy\n");
					else
                    {
                        int _sum=0;
                        for(int i=0;i<2;i++){
                            _sum += _input.read_no_eof() << i;
                        //  printf("sum is %d when i is %d ",_sun,i);
                        }
                        runLen = _sum + 3;
                        _sum =0;
                        runVal = codeLens[codeLensIndex - 1];
                    }
				}
				else if(sym == 17){
                    int _sum=0;
                    int reof;
                    for(int i=0;i<3;i++){
                        reof = _input.read_no_eof();
                        _sum +=  reof << i;
                       // printf("\nsum is %d when i is %d and input = %d ",_sun,i,reof);
                    }
                    runLen = _sum + 3;
                    //printf("runLen is %d",runLen);
                    _sum =0;
				}
				else if(sym == 18){
                     int _sum=0;
                    for(int i=0;i<7;i++){
                        _sum += _input.read_no_eof() << i;
                        //  printf("sum is %d when i is %d ",_sun,i);
                    }
                    runLen = _sum + 11;
                    _sum =0;
				}
				else{
                    printf("\nError : Symbol out of range\n");
				}
				int endd = codeLensIndex+runLen;
				if (endd > numLitLenCodes+numDistCodes){
                    printf("\nError : Run exceeds number of codes\n");
                    break;
				}
				for(int i=codeLensIndex;i<endd;i++)
                    codeLens[i] = runVal;
				//Arrays.fill(codeLens, codeLensIndex, end, runVal);

				codeLensIndex = endd;

			}
    }
    printf("\n");
    for(int i=0;i<numLitLenCodes+numDistCodes;i++)
        printf("%d ",codeLens[i]);

    int litLenCodeLen[numLitLenCodes];
    for(int i=0;i<numLitLenCodes;i++)
        litLenCodeLen[i] = codeLens[i];
    CanonicalCode litLenCode = CanonicalCode(litLenCodeLen,numLitLenCodes);
    int distCodeLen[numDistCodes];
    for(int i=0;i<numDistCodes;i++)
        distCodeLen[i] = codeLens[numLitLenCodes+i];
/*
    for(int i=0;i<numDistCodes;i++)
        printf("\n distCodeLen : %d",distCodeLen[i]);
*/
    if((numDistCodes == 1) && (distCodeLen[0] == 0)){
        int na[] = {};
        CanonicalCode distCode = CanonicalCode(na,0);
    }
    else{
        int oneCount = 0;
        int otherPositiveCount  = 0;
        for(int i=0;i<numDistCodes;i++){
            if(distCodeLen[i] == 1)
                oneCount++;
            else if(distCodeLen[i] > 1)
                otherPositiveCount++;
        }
        if ((oneCount == 1) && (otherPositiveCount == 0)){
            int distCodeLen2[32];
            for(int i=0;i<31;i++)
                distCodeLen2[i] = 0;
            for(int i=0;i<numDistCodes;i++)
                distCodeLen2[i] = distCodeLen[i];
            distCodeLen2[31] =1;
            CanonicalCode distCode = CanonicalCode(distCodeLen2,32);
            CanonicalCode a[] = {litLenCode,distCode};
            return a;
        }
        else{
            CanonicalCode distCode = CanonicalCode(distCodeLen,numDistCodes);
            CanonicalCode a[] = {litLenCode,distCode};
            return a;
        }
    }

};

int decodeRunLength(int sym, BitInputStream &_input){
    if (sym < 257 || sym > 287){
        printf("\nInvalid run length symbol: %d\n",sym);
        return -1;
    }
    else if(sym <= 264){
        return sym - 254;
    }
    else if(sym <= 284){
        int numExtraBits = (sym - 261) / 4;
        int _sum=0;
        for(int i=0;i<numExtraBits;i++){
            _sum += _input.read_no_eof() << i;
          //  printf("sum is %d when i is %d ",_sum,i);
        }
        return (((sym - 265) % 4 + 4) << numExtraBits) + 3 + _sum;
    }
    else if(sym <= 285){
        return 258;
    }
    else {
        printf("\nReserved length symbol: %d\n",sym);
        return -1;
    }
}

int decodeDistance(int sym,BitInputStream &_input){
    if (sym < 0 || sym > 31){
        printf("\nInvalid distance symbol: %d\n",sym);
        return -1;
    }
    if (sym <= 3)
			return sym + 1;
    else if (sym <= 29) {
			int numExtraBits = sym / 2 - 1;
            int _sum=0;
            for(int i=0;i<numExtraBits;i++){
                _sum += _input.read_no_eof() << i;
            //  printf("sum is %d when i is %d ",_sum,i);
            }
			return ((sym % 2 + 2) << numExtraBits) + 1 + _sum;
    } else{
        printf("\nReserved distance symbol: %d\n",sym);
        return -1;
    }
}


void decompressHuffmanBlock(CanonicalCode litLenCode, CanonicalCode distCode, BitInputStream &_input, ByteHistory &_dictionary, unsigned char * output,int oindex){
    while (true) {

            int sym = decode_next_symbol(_input,litLenCode);
            if(sym == 256)
                break;
            if(sym < 256){
                output[oindex] = (unsigned char)sym;
                oindex +=1;
                _dictionary.append(sym);
            }
            else {
                int run = decodeRunLength(sym,_input);
                if (run < 3 || run > 258)
                    printf("\nInvalid run length\n");
                if (distCode.numSymbolsAllocated == 0)
                    printf("\nLength symbol encountered with empty distance code\n");
                int distSym = decode_next_symbol(_input,distCode);
                int dist = decodeDistance(distSym,_input);
                if (dist < 1 || dist > 32768)
                    printf("\ninvalid distance \n");
                _dictionary._copy(dist,run,output,oindex);

            }

    }
}

void decompressUncompressedBlock(BitInputStream &_input, ByteHistory &_dictionary,unsigned char * out,int &oindex){
    while(_input.get_bit_position()!=0){
            _input.read_no_eof();
    }
    int _sum=0;
    for(int i=0;i<16;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    int len = _sum;
    _sum=0;
    for(int i=0;i<16;i++){
        _sum += _input.read_no_eof() << i;
        //  printf("sum is %d when i is %d ",_sum,i);
    }
    int nlen = _sum;
    if ((len ^ 0xFFFF) != nlen)
        printf("\nData format exception : Invalid length in uncompressed block\n");
    for (int i = 0; i < len; i++) {
			int b = _input.read_byte();
			if (b == -1){
                printf("\nEOFException \n");
			}
			//***********************************output.write(b);
			_dictionary.append(b);
			out[oindex] = (unsigned char) b;
			oindex +=1;
    }
}



 void Decompressor(BitInputStream &bitin,unsigned char * out){
    printf("\nHello from decompressor :\n");
    BitInputStream _input = bitin;
    unsigned char * _output = out;
    int oindex = 0;
    /*/**************************************************
    for(int i =0;i<55;i++)
    {
        printf("\n");
        cout << _output[i];
    }
    */
    ByteHistory _dictionary = ByteHistory();
//Process the stream of blocks
    bool isfinal;
    do{
        isfinal = (_input.read_no_eof() == 1); //bfinal

        //_type = input.read_int(2)
        int _sum=0;
        for(int i=0;i<2;i++){
            _sum += _input.read_no_eof() << i;
          //  printf("sum is %d when i is %d ",_sum,i);
        }
        int type = _sum;
        //

        printf("\n here is type value : %d\n",type);

        if(type == 0){
            decompressUncompressedBlock(_input,_dictionary,out,oindex);
        }
        else if(type == 1){
            //decompressHuffmanBlock(,_input,_dictionary);
            int temp_array[288]= {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8};
            int temp_array2[32]= {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
            CanonicalCode FIXED_LITERAL_LENGTH_CODE =  CanonicalCode(temp_array,288);
            CanonicalCode FIXED_DISTANCE_CODE = CanonicalCode(temp_array2,32);
            decompressHuffmanBlock(FIXED_LITERAL_LENGTH_CODE,FIXED_DISTANCE_CODE,_input,_dictionary,_output,oindex);
        }
         else if(type == 2){

            CanonicalCode * litLenAndDist = _decode_huffman_codes(_input);
            /*for(int i=0;i<numSymbolsAllocated;i++){
                printf("\nCode : %d Symbol : %d",symbolCodeBits[i],symbolValues[i]);
            }*//*
            for(int i=0;i<litLenAndDist[0].numSymbolsAllocated;i++){
                printf("\nCode : %d Symbol : %d",litLenAndDist[0].symbolCodeBits[i],litLenAndDist[0].symbolValues[i]);
            }
            for(int i=0;i<litLenAndDist[1].numSymbolsAllocated;i++){
                printf("\nCode : %d Symbol : %d",litLenAndDist[1].symbolCodeBits[i],litLenAndDist[1].symbolValues[i]);
            }*/
//******************************************************************************************************
            decompressHuffmanBlock(litLenAndDist[0],litLenAndDist[1],_input,_dictionary,_output,oindex);
        }
         else if(type == 3){
            printf("\nReserved block type\n");
        }
         else{
            printf("\nImpossible value for type\n");
        }
    }while(!isfinal);
};
int main(int argc, char**argv)
{
    FILE *fileptr;
    unsigned char *buffer;
    unsigned char compmeth,flag,extraflags,osbyte;
    long filelen;
    char infile[] = "input.gz";
    char outfile[] = "output";

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
    int temp16,tmp162;
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
        case 12: printf("\nOperating system: QDaOS");break;
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
        temp16 = ((unsigned char)buffer[filelen-4] | ((unsigned char)buffer[filelen-3]) << 8);
        tmp162 =((unsigned char)buffer[filelen-2] | ((unsigned char)buffer[filelen-1]) << 8);
        uint32_t decompsize = temp16 | (tmp162 << 16);
        unsigned char *bdecom;

        unsigned char out[decompsize];
        bdecom = (unsigned char *)malloc((filelen+1-read_index)*sizeof(char));
        for(int i=0;i<filelen+1-read_index;i++)
            bdecom[i] = buffer[read_index+i];
        for(int i=0;i<filelen+1-read_index;i++)  {
            printf("%02x ",(unsigned char)bdecom[i]);
        }
        for(int i=0;i<decompsize;i++)
            out[i] = '0';
        BitInputStream bitin = BitInputStream(bdecom);
        Decompressor(bitin,out);
       // unsigned char decomp = d.decompress_to_bytes(bitin);

        printf("\n");

        uint32_t crc,_size;
        temp16 = ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8);
        read_index +=2;

        tmp162 = ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8);
        crc = temp16 | (tmp162 << 16);
        read_index +=2;


        temp16 = ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8);
        read_index +=2;

        tmp162 = ((unsigned char)buffer[read_index] | ((unsigned char)buffer[read_index+1]) << 8);
        _size = temp16 | (tmp162 << 16);
        read_index +=2;

        printf("\noutta here : ");
        for(int i=0;i<decompsize;i++){
            cout << out[i];
        }
        FILE * pFile;
        pFile = fopen ("output", "wb");
        fwrite (out , sizeof(unsigned char),decompsize , pFile);
        fclose (pFile);

        }
        catch (exception& e){
            cout <<"Standard exception: " <<  e.what() << '\n';
        }
    return 0;
}
