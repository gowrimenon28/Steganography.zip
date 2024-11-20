# Steganography.zip

Abstract:

Steganography is the art of hiding the information in other information. Many different carrier file formats can be used, but digital images are most popular because of their frequency. For hiding the secret information in images, there exist a large variety of techniques.

Steganography is the practice of hiding the private or sensitive information within something that appears to be nothing out to the usual. In steganography, if we hide the data here no one can know that hidden data is present.Any person will not attempt to decode the information.

Requirements:

The application accepts an image file(.bmp) along with any other file which contains the message to be steged.
Analyze the size of the message file and check the capacity of the input image file(.bmp) whether the message could fit in the provided image or not
The application should provide the option to encode the file and to decode the file
To encode the file we have to pass encode option, input image file and secret file then it will create one encoded image file
To decode the file we have to pass decode option, encoded image file and secret_output file to store the secret content
This has to be a command line application and all the option has to be passed as a command line argument
Pre_requisite:

Knowledge of Encryption and Decryption
File I/O
Pointers and Structures
Bitwise Operations
