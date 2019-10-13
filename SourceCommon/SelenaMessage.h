#ifndef SELENA_MESSAGE_H
#define SELENA_MESSAGE_H
//
#	ifdef _LIB

#		include <string>

#		ifdef __cplusplus
		extern "C" {
#		endif // #ifdef __cplusplus

			void MessageToServer(
				const std::string message,
				const std::string fileMsg);

			void MessageToClient(
				const std::string message, 
				int type);

#		ifdef __cplusplus
		}
#		endif // #ifdef __cplusplus

#	endif // ifdef _LIB

//
#endif // #ifndef SELENA_MESSAGE_H
