//
//  main.cpp
//  opencv
//
//  Created by Gaspar Gyorgy on 30/05/16.
//  Copyright © 2016 Gaspar Gyorgy. All rights reserved.
//

#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include <string>
#include<fstream>
#include<sstream>

#include <dirent.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "boost/filesystem.hpp" 

using namespace boost::filesystem;
using namespace cv;
namespace fs = boost::filesystem;

class FileUtil{
    
public:
    static std::streampos fileSize( const char* filePath ){
        
        std::streampos fsize = 0;
        std::ifstream file( filePath, std::ios::binary );
        
        fsize = file.tellg();
        file.seekg( 0, std::ios::end );
        fsize = file.tellg() - fsize;
        file.close();
        
        return fsize;
    }
    
    static
    std::ifstream::pos_type filesize(const char* filename)
    {
        std::streampos fsize = 0;
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        //std::streambuf* buf_ptr =  in.rdbuf();
        fsize = in.tellg() - fsize;

        std::cout << "fsize : " << fsize << std::endl;
        return fsize;
    }

    std::vector<String> list_of_filesnames;

    void listdir(const char* dirname, int lvl)
    {
        
        int i;
        DIR* directory;
        struct dirent* entry;
        char longest_name[4096];
        
        while( (directory = opendir(dirname)) == NULL) {
            fprintf(stderr, "Couldn't open directory: %s. Possible reason is the directory does not exist!\n", dirname);
            exit(-1);
        }
        
        while((entry=readdir(directory)) != NULL) {
            
            /* Don't descend up the tree or include the current directory */
            if(strncmp(entry->d_name, "..", 2) != 0 &&
               strncmp(entry->d_name, ".", 1) != 0) {
                
                /* If it's a directory print it's name and recurse into it */
                if (entry->d_type == DT_DIR) {
                    for(i=0; i < 2*lvl; i++) {
                        printf(" ");
                    }
                    printf("%s (d)\n", entry->d_name);
                    
                    /* Prepend the current directory and recurse */
                    strncpy(longest_name, dirname, 4095);
                    strncat(longest_name, "/", 4095);
                    strncat(longest_name, entry->d_name, 4095);
                    listdir(longest_name, lvl+1);
                }
                else {
                    /* Prints the filenames inside the directory */
                    /* Print some leading space depending on the directory level */
                    for(i=0; i < 2*lvl; i++) {
                        printf(" ");
                    }
                    /* We add the names of files to a vector to use it later */
                    list_of_filesnames.push_back(entry->d_name);
                    printf("%s\n", entry->d_name);
                }
            }
        }
        
        closedir(directory);
        
        return;
    }
    
    FileUtil();
    
   ~FileUtil();

};

// Member functions definitions including constructor
FileUtil::FileUtil(void)
{
    std::cout << "Object is being created" << std::endl;
}

FileUtil::~FileUtil(void)
{
    std::cout << "Object is being deleted" << std::endl;
}

int main(int argc, const char * argv[]) {
    
    FileUtil fileutil;
    
    std::cout << CV_VERSION << "\n";
    
    fileutil.listdir("/Users/georgegaspar/Pictures/movies", 0);
    
    String moviename = fileutil.list_of_filesnames.at(16);
    std::cout << moviename << std::endl;
    
    try
    {
        
    //TODO: iterate the folder, get size of the image, and decide if resize is neccessary

        Mat image;
        image = imread( "/Users/georgegaspar/Pictures/movies/interstellar.jpg", CV_LOAD_IMAGE_UNCHANGED );
        
        std::cout << "Width : " << image.cols << std::endl;
        std::cout << "Height : " << image.rows << std::endl;
        
        std::streampos fsize_;
        fsize_ = FileUtil::filesize("/Users/georgegaspar/Pictures/movies/new_interstellar.png");
        std::cout << "File size: " << fsize_ << " bytes" << std::endl;

        
        Mat dst;
        // image resizing
        resize(image,dst, cv::Size(image.cols * 0.75,image.rows * 0.75), 0, 0, CV_INTER_LINEAR);
        
    
        // compression settings
        // we use vector out of convinience
        std::vector<int> compression_params;
        compression_params.push_back(IMWRITE_PNG_COMPRESSION);
        compression_params.push_back(9);
        
        std::vector<int> params;
        params.push_back(CV_IMWRITE_JPEG_QUALITY);
        params.push_back(10);
        
        // we create a vector for buffer (vector doesn't have fix size, it grows along)
        std::vector<unsigned char> buf;

        cv::imencode("*.jpg", dst, buf, params);
        std::cout << "Buffer Size : " << BUFSIZ << "," << sizeof(buf) << std::endl;
        
    imwrite( "/Users/georgegaspar/Pictures/movies/new_interstellar.png", dst, compression_params);

    std::streampos fsize;
    fsize = FileUtil::filesize("/Users/georgegaspar/Pictures/movies/new_interstellar.png");
    std::cout << "File size: " << fsize << " bytes" << std::endl;
        
    IplImage newImage;
    newImage =  *cvLoadImage("/Users/georgegaspar/Pictures/movies/interstellar.jpg", CV_LOAD_IMAGE_UNCHANGED);
        
        CvMat encImage = *cvEncodeImage("*.jpg", &newImage, 0 );
        cvDecodeImageM(&encImage, CV_LOAD_IMAGE_UNCHANGED);

        std::cout << "Size : " << newImage.imageSize << " bytes" << std::endl;
        
        newImage.~_IplImage();
    }
    
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;
    }
    
    
    std::cout << "Done \n";
    
    /*
     IplImage* sorceImage = cvLoadImage( "/Users/georgegaspar/Pictures/movies/interstellar.jpg" );
     
     cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE );
     cvShowImage("Example1", sorceImage );
     
     cvWaitKey(1000);
     cvReleaseImage( &sorceImage );
     cvDestroyWindow("Example1");
     */
    
    /*
     Mat gray_image;
     // grayscale conversion
     cvtColor( image, gray_image, CV_BGR2GRAY );
     
     imwrite( "/Users/georgegaspar/Pictures/movies/interstellar2.jpg", gray_image);
     */
    
    return 0;
    
}