//
// Created by Gabriel Mitterrutzner on 15/01/23.
//

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <thread>
#include <vector>

constexpr int default_megabyte_nums = 64;
constexpr int readWriteOpNum = 9;
constexpr int megabyte = 1000000;

int main(int argc, char** argv) {
	int numProcs, myRank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	long megaByteSize = default_megabyte_nums;
	if(argc == 2) {
		megaByteSize = std::atoi(argv[1]);
		if(megaByteSize * numProcs > INT32_MAX) {
			if(myRank == 0) {
				std::cerr << "can just write int32 maximum elements" << std::endl;
			}
			MPI_Finalize();
			return EXIT_FAILURE;
		}
	}

	megaByteSize *= megabyte;

	// make custom contiguous type for sending more than int32 max elements
	MPI_Datatype customType;
	MPI_Type_contiguous(megaByteSize, MPI_CHAR, &customType);
	MPI_Type_commit(&customType);

	std::vector<char> gatherContent;
	MPI_File file;

	if(myRank == 0) {
		// for this application it wouldn't make sense to continue, therefore also disregarding
		// return values is possible
		MPI_File_set_errhandler(MPI_FILE_NULL, MPI_ERRORS_ARE_FATAL);

		std::string filename = "temp_" + std::to_string(myRank) + ".txt";
		MPI_File_open(MPI_COMM_SELF, filename.c_str(), MPI_MODE_RDWR | MPI_MODE_CREATE,
		              MPI_INFO_NULL, &file);
		std::cout << "rank 0" << std::endl;
		std::cout << megaByteSize * numProcs << std::endl;
		std::cout << megaByteSize * numProcs / 1000000 << std::endl;

		gatherContent.resize((megaByteSize + 1) * numProcs, 0);
	}

	std::vector<char> writeContent;
	writeContent.resize(megaByteSize, (static_cast<char>(myRank) + 'A'));

	std::vector<char> readContent;
	readContent.resize(megaByteSize, 0);

	double start = MPI_Wtime();

	MPI_Gather(writeContent.data(), 1, customType, gatherContent.data(), 1, customType, 0,
	           MPI_COMM_WORLD);
	if(myRank == 0) {
		MPI_File_write(file, gatherContent.data(), 1 * numProcs, customType, MPI_STATUS_IGNORE);
		MPI_File_sync(file);
	}

	for(int i = 0; i < readWriteOpNum; i++) {
		if(myRank == 0) {
			MPI_File_seek(file, 0, MPI_SEEK_SET);
			MPI_File_read(file, gatherContent.data(), 1 * numProcs, customType, MPI_STATUS_IGNORE);
		}

		MPI_Scatter(gatherContent.data(), 1, customType, readContent.data(), 1, customType, 0,
		            MPI_COMM_WORLD);

		if(!std::equal(writeContent.begin(), writeContent.end(), readContent.begin())) {
			std::cerr << "error contents were not equal" << std::endl;
			MPI_File_close(&file);
			MPI_Finalize();
			return EXIT_FAILURE;
		}

		MPI_Gather(writeContent.data(), 1, customType, gatherContent.data(), 1, customType, 0,
		           MPI_COMM_WORLD);

		if(myRank == 0) {
			MPI_File_seek(file, 0, MPI_SEEK_SET);
			MPI_File_write(file, gatherContent.data(), 1 * numProcs, customType, MPI_STATUS_IGNORE);
			MPI_File_sync(file);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	double elapsed = MPI_Wtime() - start;
	double memoryUsed =
	    (readWriteOpNum * 2 + 1) * ((double)megaByteSize / (double)megabyte) * numProcs;

	if(myRank == 0) {
		std::cout << "Elapsed: " << elapsed << " , Bandwidth: " << memoryUsed / elapsed
		          << std::endl;

		MPI_File_close(&file);
	}

	MPI_Type_free(&customType);
	MPI_Finalize();

	return EXIT_SUCCESS;
}
