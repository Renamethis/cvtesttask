#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string_view>
#include <dav_class.h>
#define MIN_AREA 100
inline bool exists (const std::string& filename) {
    std::ifstream f(filename);
    return f.good();
}
int main(int, char**) {
    using json = nlohmann::json;
    // Загрузка видео-файла
    DAV video = DAV("Video/Test.dav");
    std::string fileTemplateInput = "Video/json/Test_%d.json";
    std::string fileTemplateOutput = "Video/json_output/Test_%d.json";
    // Запускаем отображение и ожидаем ввода клавиши space
    cv::imshow("Player", video.cadr(1));
    while(cv::waitKey(30) != ' ') ;
    // Проход по кадрам видео
    for(int i = 1; i < video.CountKadr; i++) {
        cv::Mat currentFrame = video.cadr(i+1);
        char fileInput[fileTemplateInput.length() - 2 + std::to_string(i).length()];
        sprintf(fileInput, fileTemplateInput.c_str(), i+1);
        if(exists(fileInput)) {
            std::ifstream ifile(fileInput);
            if(ifile.is_open()) {
                // Парсим json и ищем точки прямоугольника
                json parsed, output;
                ifile >> parsed;
                output = parsed;
                json shapes = parsed.at("shapes");
                for(auto j = shapes.begin(); j != shapes.end(); j++) {
                    if((*j).at("shape_type") == std::string("rectangle")) {
                        json jsonPoints = (*j).at("points");
                        std::vector<cv::Point> points;
                        for(auto k = jsonPoints.begin(); k != jsonPoints.end(); k++)
                            points.push_back(cv::Point((*k).at(0), (*k).at(1)));
                        if(points.size() != 2) 
                            throw std::runtime_error("Too much points for rectangle!");
                        // Бинаризируем изображение по цвету в цветовой схеме HSV
                        cv::Rect rect(points[0], points[1]);
                        cv::Mat roi = currentFrame(rect), blured1;
                        cv::Mat mask1, mask2, result;
                        cv::cvtColor(roi, result, cv::COLOR_BGR2HSV);
                        cv::inRange(result, cv::Scalar(0, 70, 50), cv::Scalar(20, 255, 255), mask1);
                        cv::inRange(result, cv::Scalar(170, 70, 50), cv::Scalar(180, 240, 255), mask2);
                        cv::bitwise_or(mask1, mask2, result);
                        std::vector<std::vector<cv::Point>> contours;
                        std::vector<cv::Vec4i> hierarchy;
                        // Определяем контуры на бинарном изображении и ищем максимальный по площади
                        findContours(result, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
                        int pos = -1;
                        double maxArea = 0.0;
                        for(auto k = contours.begin(); k != contours.end(); k++) {
                            double currentArea = cv::contourArea(*k);
                            if(currentArea > maxArea) {
                                maxArea = currentArea;
                                pos = k - contours.begin();
                            }
                        }
                        // Отрисовываем контуры и распознанные знаки
                        cv::rectangle(currentFrame, rect, cv::Scalar(0, 255, 0));
                        if(pos != -1 && cv::contourArea(contours[pos]) > MIN_AREA) {
                            cv::drawContours(roi, contours, pos, cv::Scalar(0, 0, 255), 2, cv::LINE_8, hierarchy, 0);
                            // Записываем результаты в json-объект
                            (*j)["shape_type"] = "polygon";
                            (*j)["points"] = json::array();
                            for(auto k = contours[pos].begin(); k != contours[pos].end(); k++) {
                                cv::Point point = (*k);
                                (*j)["points"].push_back({point.x, point.y});
                            }
                            output["shapes"].push_back(*j);
                        }
                    }
                }
                // Сериализуем результаты
                char fileOutput[fileTemplateOutput.length() - 2 + std::to_string(i).length()];
                sprintf(fileOutput, fileTemplateOutput.c_str(), i+1);
                std::ofstream ofile(fileOutput);
                if(ofile.good() && ofile.is_open()) {
                    output >> ofile;
                }
            }
        }
        // Отображение результатов
        cv::imshow("Player", currentFrame);
        auto key = cv::waitKey(30);
        if(key == 'q') // Выход из программы
            break;
        else if(key == ' ') // Пауза
            while(cv::waitKey(30) != ' ')
                cv::imshow("Player", currentFrame);
    }
    cv::destroyAllWindows();
    return 0;
}