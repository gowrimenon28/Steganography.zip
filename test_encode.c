#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc , char *argv[])
{
    EncodeInfo encInfo;
	DecodeInfo  decInfo ;
    uint img_size;

   
 
	//check the operation type -e or -d
	if(check_operation_type(argv) == e_encode)
	{
		printf("---------Selected Encoding----------\n") ;
		//read and validate command line argument
       if(read_and_validate_encode_args( argv, &encInfo) == e_success)
	   {
		   printf("read and validate is successful :)\n") ;
		   if(do_encoding(&encInfo) == e_success)
		   {
			   printf("Hurray....Encoding is completed\n") ;
		   }
		   else
		   {
			   printf("Failure:Encoding is not completed\n") ;
		   }
	  }
	   else
		   printf("Read and Validate is unsuccesful:(\n") ;
	}
	else if(check_operation_type(argv) == e_decode)
	{
		printf("---------Selected Decoding----------\n") ;                                                     
         if ( read_and_validate_decode_args ( argv , &decInfo ) == e_success )
		 {
			 do_decoding( &decInfo );                                                                                                                                                                                   }      
	}                                                                                                            
	else 
	{
		printf("invalid Option\n********Usage********\n") ;
		printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n") ;
		printf("Decoding: ./a.out -d stego.bmp\n") ;
	}

    return 0;
}
OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e") == 0)
		return e_encode ;
	else if(strcmp(argv[1],"-d") == 0)
		return e_decode ;
	else 
		return e_unsupported ;
}

