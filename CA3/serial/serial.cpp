#include "define.hpp"
#include "shared_functions.hpp"


vector<Hidden_Node> hidden_nodes(NUMBER_OF_HIDDEN_CELLS);
vector<Output_Node> output_nodes(NUMBER_OF_OUTPUT_CELLS);

void locateCursor(const int row, const int col){
    printf("%c[%d;%dH",27,row,col);
}

/**
 * @details Clear terminal screen by printing an escape sequence
 */

void clearScreen(){
    printf("\e[1;1H\e[2J");
}

/**
 * @details Outputs a 28x28 MNIST image as charachters ("."s and "X"s)
 */

void displayImage(MNIST_Image *img, int row, int col){

    char imgStr[(MNIST_IMG_HEIGHT * MNIST_IMG_WIDTH)+((col+1)*MNIST_IMG_HEIGHT)+1];
    strcpy(imgStr, "");

    for (int y=0; y<MNIST_IMG_HEIGHT; y++){

        for (int o=0; o<col-2; o++) strcat(imgStr," ");
        strcat(imgStr,"|");

        for (int x=0; x<MNIST_IMG_WIDTH; x++){
            strcat(imgStr, img->pixel[y*MNIST_IMG_HEIGHT+x] ? "X" : "." );
        }
        strcat(imgStr,"\n");
    }

    if (col!=0 && row!=0) locateCursor(row, 0);
    printf("%s",imgStr);
}

/**
 * @details Outputs a 28x28 text frame at a defined screen position
 */

void displayImageFrame(int row, int col){

    if (col!=0 && row!=0) locateCursor(row, col);

    printf("------------------------------\n");

    for (int i=0; i<MNIST_IMG_HEIGHT; i++){
        for (int o=0; o<col-1; o++) printf(" ");
        printf("|                            |\n");
    }

    for (int o=0; o<col-1; o++) printf(" ");
    printf("------------------------------");

}

/**
 * @details Outputs reading progress while processing MNIST testing images
 */

void displayLoadingProgressTesting(int imgCount, int y, int x){

    float progress = (float)(imgCount+1)/(float)(MNIST_MAX_TESTING_IMAGES)*100;

    if (x!=0 && y!=0) locateCursor(y, x);

    printf("Testing image No. %5d of %5d images [%d%%]\n                                  ",(imgCount+1),MNIST_MAX_TESTING_IMAGES,(int)progress);

}

/**
 * @details Outputs image recognition progress and error count
 */

void displayProgress(int imgCount, int errCount, int y, int x){

    double successRate = 1 - ((double)errCount/(double)(imgCount+1));

    if (x!=0 && y!=0) locateCursor(y, x);

    printf("Result: Correct=%5d  Incorrect=%5d  Success-Rate= %5.2f%% \n",imgCount+1-errCount, errCount, successRate*100);


}


void allocateHiddenParameters(){
    int idx = 0;
    int bidx = 0;
    ifstream weights(HIDDEN_WEIGHTS_FILE);
    for(string line; getline(weights, line); )   //read stream line by line
    {
        stringstream in(line);
        for (int i = 0; i < 28*28; ++i){
            in >> hidden_nodes[idx].weights[i];
      }
      idx++;
    }
    weights.close();

    ifstream biases(OUTPUT_BIASES_FILE);
    for(string line; getline(biases, line); )   //read stream line by line
    {
        stringstream in(line);
        in >> hidden_nodes[bidx].bias;
        bidx++;
    }
    biases.close();
}

/**
 * @brief allocate weights and bias to respective output cells
 */

void allocateOutputParameters(){
    int idx = 0;
    int bidx = 0;
    ifstream weights(OUTPUT_WEIGHTS_FILE); //"layersinfo.txt"
    for(string line; getline(weights, line); )   //read stream line by line
    {
        stringstream in(line);
        for (int i = 0; i < 256; ++i){
            in >> output_nodes[idx].weights[i];
      }
      idx++;
    }
    weights.close();

    ifstream biases(OUTPUT_BIASES_FILE);
    for(string line; getline(biases, line); )   //read stream line by line
    {
        stringstream in(line);
        in >> output_nodes[bidx].bias;
        bidx++;
    }
    biases.close();

}

/**
 * @details The output prediction is derived by finding the maxmimum output value
 * and returning its index (=0-9 number) as the prediction.
 */

int getNNPrediction(){

    double maxOut = 0;
    int maxInd = 0;

    for (int i=0; i<NUMBER_OF_OUTPUT_CELLS; i++){

        if (output_nodes[i].output > maxOut){
            maxOut = output_nodes[i].output;
            maxInd = i;
        }
    }

    return maxInd;

}

/**
 * @details test the neural networks to obtain its accuracy when classifying
 * 10k images.
 */

void testNN(int imageCount){
        // open MNIST files
    FILE *imageFile, *labelFile;
    imageFile = openMNISTImageFile(MNIST_TESTING_SET_IMAGE_FILE_NAME);
    labelFile = openMNISTLabelFile(MNIST_TESTING_SET_LABEL_FILE_NAME);


    // screen output for monitoring progress
    // displayImageFrame(7,5);

    // number of incorrect predictions
    int errCount = 0;
    int curCount = 0;


    // Loop through all images in the file
    for (int imgCount=0; imgCount< imageCount; imgCount++){

        // Reading next image and corresponding label
        MNIST_Image img = getImage(imageFile);
        MNIST_Label lbl = getLabel(labelFile);


        // loop through all output cells for the given image
        for (int i= 0; i < NUMBER_OF_OUTPUT_CELLS; i++){
            output_nodes[i].output = 0;
            for (int j = 0; j < NUMBER_OF_HIDDEN_CELLS; j++) {
                hidden_nodes[j].output = 0;
                for (int z = 0; z < NUMBER_OF_INPUT_CELLS; z++) {
                    hidden_nodes[j].output += img.pixel[z] * hidden_nodes[j].weights[z];
                }
                hidden_nodes[j].output += hidden_nodes[j].bias;
                hidden_nodes[j].output = (hidden_nodes[j].output >= 0) ?  hidden_nodes[j].output : 0;
                output_nodes[i].output += hidden_nodes[j].output * output_nodes[i].weights[j];
            }
            
            output_nodes[i].output += 1/(1+ exp(-1* output_nodes[i].output));
        }        
        

        int predictedNum = getNNPrediction();
        if (predictedNum!=lbl) errCount++;
        else curCount++;

        displayLiveProgress(3, predictedNum, lbl, curCount, imgCount + 1, "Serial");

    }

    // Close files
    fclose(imageFile);
    fclose(labelFile);

}

int main(int argc, const char * argv[]) {

        if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <images_count>\n";
        return 1;
    }

    int imageCount = std::stoi(argv[1]);

    // alocating respective parameters to hidden and output layer cells
    allocateHiddenParameters();
    allocateOutputParameters();

    //test the neural network
    testNN(imageCount);

    return 0;
}
