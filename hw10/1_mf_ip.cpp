#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

constexpr int default_byte_nums = 4;
constexpr int readWriteOpNum = 9;

int main(int argc, char** argv) {

	int numProcs, myRank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	int megaByteSize = default_byte_nums;
	if(argc == 2) {
		megaByteSize = std::atoi(argv[1]);
	}

	megaByteSize *= 1000000;

	if(megaByteSize % numProcs != 0) {
		if(myRank == 0) {
			std::cerr << "byte size must be a multiple of N" << std::endl;
		}
		MPI_Finalize();
	}

	int bytePerRank = megaByteSize / numProcs;

	// for this application it wouldn't make sense to continue, therefore also disregarding return
	// values is possible
	MPI_File_set_errhandler(MPI_FILE_NULL, MPI_ERRORS_ARE_FATAL);

	std::string filename = "temp_" + std::to_string(myRank);
	MPI_File file;
	MPI_File_open(MPI_COMM_SELF, filename.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL,
	              &file);

	std::vector<char> writeContent;

	writeContent.resize(bytePerRank + 1, (static_cast<char>(myRank) + 'A'));
	writeContent[writeContent.size() - 1] = 0;
	std::vector<char> readContent;
	readContent.resize(bytePerRank + 1, 1);
	readContent[readContent.size() - 1] = 0;
	std::cout << writeContent.max_size() << std::endl;
	double start = MPI_Wtime();
	MPI_File_write(file, writeContent.data(), bytePerRank, MPI_CHAR, MPI_STATUS_IGNORE);

	for(int i = 0; i < readWriteOpNum; i++) {
		MPI_File_seek(file, 0, MPI_SEEK_SET);
		MPI_File_read(file, readContent.data(), bytePerRank, MPI_CHAR, MPI_STATUS_IGNORE);

		if(std::strcmp(readContent.data(), writeContent.data())) {
			std::cerr << "error contents were not equal" << std::endl;
			MPI_File_close(&file);
			MPI_Finalize();
			return EXIT_FAILURE;
		}

		MPI_File_seek(file, 0, MPI_SEEK_SET);
		MPI_File_write(file, writeContent.data(), bytePerRank, MPI_CHAR, MPI_STATUS_IGNORE);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double elapsed = MPI_Wtime() - start;
	double memoryUsed = 19 * megaByteSize;

	if(myRank == 0) {
		std::cout << "Elapsed: " << elapsed << " , Bandwidth: " << memoryUsed / elapsed
		          << std::endl;
	}

	MPI_File_close(&file);
	MPI_Finalize();

	return EXIT_SUCCESS;
}