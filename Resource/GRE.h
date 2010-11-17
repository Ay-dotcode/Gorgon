#pragma once

#include "../Utils/GGE.h"
#include "../Utils/Utils.h"
#include <stdio.h>
#include <string>
#ifdef _DEBUG
#include <assert.h>
#endif

using namespace std;
using namespace gge;

////This namespace contains Gorgon Resource System.
namespace gre {

	////The Guid class is used for Guid operations
	class Guid {
	protected:
		BYTE *bytes;
		int *ints;

	public:
		////Empty constructor
		Guid() {
			init();
		}

		Guid(Guid &guid) {
			init();

			ints[0]=guid.ints[0];
			ints[1]=guid.ints[1];
			ints[2]=guid.ints[2];
			ints[3]=guid.ints[3];
		}

		////Creates a guid from given integers
		Guid(int p1, int p2, int p3, int p4) {
			init();

			ints[0]=p1;
			ints[1]=p2;
			ints[2]=p3;
			ints[3]=p4;
		}

		////Creates a guid from the give array, if NULL is given
		/// an empty guid is created
		Guid(BYTE *bytes) {
			init();

			if(!bytes)
				Empty();
			else {
				int i;
				for(i=0;i<16;i++)
					this->bytes[i]=bytes[i];
			}
		}

		////Reads a guid from a file
		Guid(FILE* file) {
			init();

			fread(bytes, 1, 16, file);
		}

		////Reads a guid from a file
		void ReadFrom(FILE* file) {
			fread(bytes, 1, 16, file);
		}

		////Deletes used data
		~Guid() {
			delete bytes;
		}

		////Empties this guid
		inline void Empty() {
			ints[0]=0;
			ints[1]=0;
			ints[2]=0;
			ints[3]=0;
		}

		////Checks if this guid is empty
		inline bool isEmpty() {
			if(ints[0]==0 && ints[1]==0 && ints[2]==0 && ints[3]==0)
				return true;

			return false;
		}

		////Compares two guids
		inline bool isEqual(Guid &guid) {
			if(guid.ints[0]==ints[0] && guid.ints[1]==ints[1] && guid.ints[2]==ints[2] && guid.ints[3]==ints[3])
				return true;

			return false;
		}

		////Compares two guids
		inline bool isEqual(Guid *guid) {
			if(guid==NULL)
				return isEmpty();

			if(guid->ints[0]==ints[0] && guid->ints[1]==ints[1] && guid->ints[2]==ints[2] && guid->ints[3]==ints[3])
				return true;

			return false;
		}

		////Compares two guids
		inline bool operator ==(Guid &guid) { return isEqual(guid); }

		////Compares two guids
		inline bool operator ==(Guid *guid) { return isEqual(guid); }

		////Writes this guid to a file
		inline void Write(FILE *file) {
			fwrite(bytes, 1, 16, file);
		}

	private:
		void init() {
			bytes=new BYTE[16];
			ints=(int*)bytes;
		}
	};

	class ResourceFile;
	class ResourceBase;
}