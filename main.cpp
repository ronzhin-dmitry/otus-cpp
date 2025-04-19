/**
@file main.cpp
*/
#include "lib.h"

/**
 * @brief Entry point
 *
 * Execution of the program
 * starts here.
 *
 * @param argc Number of arguments
 * @param argv List of arguments
 *
 * @return Program exit status
 */
int main(int argc, char *argv[])
{
    using namespace std;
    if (argc != 3)
    {
        cerr << "Need tree arguments" << endl;
        return 1;
    }
    string filename = argv[1];
    string model_name = argv[2];
    cout << "Running model " << model_name << " for testcase " << filename << endl;
    double acc = 0;
    size_t N = 0;
    if (model_name == "mlp")
    {
        //TODO
        //This is naive inference (vector-by-vector)
        //It can surely be optimized via batch-read (we can multiply a lot of data at once in single batch)
        //This can be a further optimization point -- but it requires implementing batch-predict
        auto w1 = read_mat_from_file(784, 128, "w1.txt");
        auto w2 = read_mat_from_file(128, 10, "w2.txt");

        auto clf = MlpClassifier{w1.transpose(), w2.transpose()};

        auto features = MlpClassifier::features_t{};

        std::ifstream test_data{"test.csv"};
        for (;;)
        {
            size_t y_true;
            test_data >> y_true;
            if (test_data.peek() == ',') test_data.ignore();
            if (!read_features(test_data, features, ','))
            {
                break;
            }
            auto y_pred = clf.predict(features);
            N++;
            if(y_pred == y_true)
                acc += 1;
        }
        cout << "Accuracy: " << acc / N << endl;
    }
    else
    {
        cerr << "Currently this model not implemented" << endl;
        return 1;
    }
    return 0;
}