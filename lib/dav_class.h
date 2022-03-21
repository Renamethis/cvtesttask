
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include <fstream>
#include <vector>


class DAV{
	
public:
	int CountKadr;
	short width;
	short height;

	
	std::ifstream dav_file;
	std::vector<int64_t> kadr_offset;

	DAV(char *file_name="Cam2.dav") {

		dav_file.open(file_name, std::fstream::binary);

		if(dav_file.is_open()){
		
			dav_file.seekg(8+8);
			char bytes2[sizeof(int64_t)]; 


			dav_file.read(bytes2, sizeof(int64_t));
			int64_t OffsetIndex; memcpy(&OffsetIndex, bytes2, sizeof(int64_t));
		

			dav_file.read(bytes2, sizeof(int64_t));
			int64_t SizeIndex; memcpy(&SizeIndex, bytes2, sizeof(int64_t));


			dav_file.seekg(OffsetIndex);

			CountKadr = int(SizeIndex/20);


			for(int i = 0; i < CountKadr; i++){
				dav_file.seekg(OffsetIndex+4+4+(4+4+8+4)*i);
			
				dav_file.read(bytes2, sizeof(int64_t));
				int64_t offset; memcpy(&offset, bytes2, sizeof(int64_t));

				kadr_offset.push_back(offset);
		
			}

			dav_file.seekg(kadr_offset[0]+4+4+4+2+2);
		
			char bytes3[sizeof(short)];
			dav_file.read(bytes3, sizeof(short));
			memcpy(&width, bytes3, sizeof(short));
		
			dav_file.read(bytes3, sizeof(short));
			memcpy(&height, bytes3, sizeof(short));
		

		}
	
	}
	~DAV(){
		dav_file.close();
	}


	cv::Mat cadr(int cadr = 1){// cadrs begin with 1
		
		dav_file.seekg(kadr_offset[cadr-1]+4+4);

		unsigned int SizeKadr; 

        char bytes4[sizeof(unsigned int)];
		dav_file.read(bytes4, sizeof(unsigned int));
		memcpy(&SizeKadr, bytes4, sizeof(unsigned int ));

		dav_file.seekg(kadr_offset[cadr-1]+4+4+4+2+2+2+2+4+4+4);
		
		std::vector<char> vectordata;
		for(int i =0; i< SizeKadr; i++){
			char buf[1];
			dav_file.read (buf,1);
			vectordata.push_back(buf[0]);

		}
		cv::Mat data_mat(vectordata, true);
		cv::Mat image(cv::imdecode(data_mat,1));

		return image;
	}

};