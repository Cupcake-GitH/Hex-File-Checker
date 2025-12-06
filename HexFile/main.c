#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define START_CODE		1U
#define BYTE_COUNT		2U
#define ADDRESS			4U
#define RECORD_TYPE		2U
#define CHECK_SUM		2U
#define END_OF_LINE		1U

typedef enum{
	ERROR_START_CODE,
	ERROR_BYTE_COUNT,
	ERROR_HEX,
	ERROR_CHECKSUM,
	ERROR,
	NO_ERROR,
}Error_type;

Error_type check_Start_Code(char *str){
	//Error_type retVal = ERROR_START_CODE;
	if (str[0] != ':'){
		return ERROR_START_CODE;
	}
	else{
		return NO_ERROR;
	}
}

char Convert_ASCII_to_DEC(char charc1, char charc2){
	char retVal = 0;
	
	if((charc1 >= '0') && (charc1 <= '9')){
		charc1 = charc1 - '0';
	}
	if((charc1 >= 'A') && (charc1 <= 'F')){
		charc1 = charc1 - 'A' + 10;
	}
	if((charc2 >= '0') && (charc2 <= '9')){
		charc2 = charc2 - '0';
	}
	if((charc2 >= 'A') && (charc2 <= 'F')){
		charc2 = charc2 - 'A' + 10;
	}
	
	retVal = ((charc1 & 0x0F) << 4) | (charc2 & 0x0F);
	
	//return retVal;
}

Error_type check_Byte_Count(char str[]){
	unsigned char byte_count = 0;
	unsigned char number_of_data = 0;
	//Error_type retVal =  NO_ERROR;
	
	byte_count = Convert_ASCII_to_DEC(str[1], str[2]);
	printf("%d\n", strlen(str));
	number_of_data = strlen(str) - START_CODE - BYTE_COUNT - ADDRESS \
								 - RECORD_TYPE - CHECK_SUM - END_OF_LINE;
								 
	printf("%d\n", byte_count);
	printf("%d\n", number_of_data);
	//printf("%c\n", byte_count);
	
	if(byte_count * 2 != number_of_data){
		return ERROR_BYTE_COUNT;
	}
	return NO_ERROR;
	//return retVal;
}

/*
void trim_newline(char str[]) {
    int len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[--len] = '\0';
    }
}*/

Error_type check_Hex(char str[])// --> check cac ky tu Hex
{
	//Error_type retVal = NO_ERROR
	//trim_newline(str);
	int i;
	
	for(i = 1; str[i] != '\n'; i++){
		char c = str[i];
		if(!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))){
			return ERROR_HEX;
		}
	}
	return NO_ERROR;
}

Error_type check_CheckSum(char str[]) // --> check sum cho
{
	int len = strlen(str);
	unsigned char sum = 0;
	int i;
	for(i = 1; i < (len - END_OF_LINE - START_CODE - CHECK_SUM); i += 2){
		sum = sum + Convert_ASCII_to_DEC(str[i], str[i + 1]);
	}
	
	sum = ~sum + 1;
	
	unsigned char checksum = Convert_ASCII_to_DEC(str[len - 3], str[len - 2]);
	
	if(sum != checksum){
		return ERROR_CHECKSUM;
	}
	
	return NO_ERROR;
}

void parse_data (char str[], int count){
	char recordType_value = 0;
	char i = 0;
	
	recordType_value = Convert_ASCII_to_DEC(str[7], str[8]);
	
	//printf("\n Record type: %d", recordType_value);
	
	if(recordType_value == 0){
		printf("Line %d: - ", count);
		
		for(i = 3; i < 7; i++){
			printf("%c", str[i]);
		}
		
		printf("  ");
		
		for(i = 9; i < 9 + Convert_ASCII_to_DEC(str[1], str[2]) * 2; i++){
			printf("%c", str[i]);
		}
		
		printf("\n");
	}	
}

int main(){
	FILE *ptr = NULL;
	char str[256];
	int count = 0;
	bool status_error = false;
	Error_type status = NO_ERROR;
	
	ptr = fopen("example.hex", "r");
	
	/*if (ptr == NULL) {
		printf("Cannot open file\n");
		return 1;
	}*/
	
	if (ptr != NULL){
    	while (fgets(str, sizeof(str), ptr)) {
        	//trim_newline(str);
			count++;
        	
        	printf("\n%s", str);
        	
        	if (status == ERROR_START_CODE) {
        		status_error = false;
            	printf("Line %d: Error start code\n", count);
        	}
        	if (status == ERROR_HEX) {
        		status_error = false;
            	printf("Line %d: Invalid hex character\n", count);
        	}
        	if (status == ERROR_BYTE_COUNT) {
        		status_error = false;
            	printf("Line %d: Byte count mismatch\n", count);
        	}
        	if (status == ERROR_CHECKSUM) {
        		status_error = false;
            	printf("Line %d: Invalid checksum\n", count);
        	}	
        	if (status_error = true){
        		parse_data(str, count);
        	}
    	}
    }
	else{
		printf("Cannot open file\n");
		return 1;
	}
	
	fclose(ptr);
	return 0;
}
