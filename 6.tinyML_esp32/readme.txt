Интерпритатор движения.

проект реализован с помощью он-лайн платформы edgeimpulse.
задача проекта - ознакомление и реализация TinyML, крошечного машинного обучения.

https://studio.edgeimpulse.com/public/234424/latest


используется датчик-акселерометр. при перемещении датчика по определенной траектории должно опознаваться движение и выполняться соответствующее ему действие. 
пример:
движение по кругу - включение нагрузки1 (освещения);
движение по линии - включение шагового мотора (открытие/закрытие штор);
без движения - разовая световая индикация (тестовый режим);
произвольное движение/шум - отключение нагрузки1 (освещение).

итоговое устройство представляет собой акселерометр, подключенный к esp32. на esp32 происходит сбор данных и определение фигуры выписываемой датчиком. 
esp32 подключена к сервису edgeimpulse, куда отправляет получнные значния с акселерометра.

обработка полученных данных производится сначала с помощью спектрального анализа, потом выполняется классификация полученных данных.