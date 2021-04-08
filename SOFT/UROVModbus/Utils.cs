using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Globalization;
using System.Reflection;
using System.Drawing.Design;
using System.Drawing;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Xml.Serialization;
using System.IO;

namespace UROVModbus
{

    /// <summary>
    /// конфиг приложения
    /// </summary>
    public class Config
    {


        private static object lockFlag = new object();
        private static Config instance;

        /// <summary>
        /// синглтон
        /// </summary>
        [XmlIgnore]
        public static Config Instance
        {
            get
            {
                lock (lockFlag)
                {
                    if (instance == null)
                    {

                        // instance = new Config();                     
                        try
                        {
                            //Пытаемся загрузить файл с диска и десериализовать его
                            using (FileStream fs =
                                new FileStream(Application.StartupPath
                                + "\\config.xml", FileMode.Open))
                            {
                                System.Xml.Serialization.XmlSerializer xs =
                                    new System.Xml.Serialization.XmlSerializer(typeof(Config));

                                instance = (Config)xs.Deserialize(fs);


                            }
                        }
                        catch (Exception)
                        {
                            //Если не удалось десериализовать то просто создаем новый экземпляр
                            instance = new Config();
                        }


                        instance.Clear();

                    } // if instance == null
                } // lock
                return instance;
            } // get

        }

        /// <summary>
        /// перезагружаем
        /// </summary>
        public static void Reload()
        {
            instance = null;
        }

        /// <summary>
        /// сохраняем в файл
        /// </summary>
        public void Save()
        {
            using (FileStream fs =
              new FileStream(Application.StartupPath + "\\config.xml", FileMode.Create))
            {
                System.Xml.Serialization.XmlSerializer xs =
                    new System.Xml.Serialization.XmlSerializer(typeof(Config));
                xs.Serialize(fs, instance);
            }
        }
        private Config()
        {
        }

        /// <summary>
        /// сброс настроек
        /// </summary>
        public void Clear()
        {
        }

        [XmlElement("ChartPhase1Visible")]
        public bool ChartPhase1Visible { get; set; }

        [XmlElement("ChartPhase2Visible")]
        public bool ChartPhase2Visible { get; set; }

        [XmlElement("ChartPhase3Visible")]
        public bool ChartPhase3Visible { get; set; }


        private int skipCounter = 1;
        [XmlIgnore]
        public int SkipCounter
        {
            get { return skipCounter; }
            set { skipCounter = value; }
        }

        private int ethalonCompareDelta = 50;
        [XmlIgnore]
        public int EthalonCompareDelta
        {
            get { return ethalonCompareDelta; }
            set { ethalonCompareDelta = value; }
        }

        private uint asuTpFlags = 0xFF;
        [XmlIgnore]
        public uint AsuTpFlags
        {
            get { return asuTpFlags; }
            set { asuTpFlags = value; }
        }




    }

    /// <summary>
    /// данные по одному каналу
    /// </summary>
    public class ChannelData
    {
        public int RecordTime = 0;
        public List<int> Values = new List<int>();

    };

    /// <summary>
    /// данные одной записи по току
    /// </summary>
    public class CurrentInfo
    {
        public int Time;
        public int Channel1;
        public int Channel2;
        public int Channel3;
    }

    /// <summary>
    /// Информация с лог-файла
    /// </summary>
    public class LogInfo
    {
        /// <summary>
        /// Список записей в лог-файле
        /// </summary>
        public List<InterruptRecord> list = new List<InterruptRecord>();
        public string addToColumnName;
        public bool computeMotoresurcePercents;
    }


    /// <summary>
    /// хэлпер для enum
    /// </summary>
    public class EnumHelpers
    {
        public static string GetEnumDescription(Enum value)
        {
            FieldInfo fi = value.GetType().GetField(value.ToString());

            DescriptionAttribute[] attributes =
                (DescriptionAttribute[])fi.GetCustomAttributes(
                typeof(DescriptionAttribute),
                false);

            if (attributes != null &&
                attributes.Length > 0)
                return attributes[0].Description;
            else
                return value.ToString();
        }

        public static T GetValueFromDescription<T>(string description)
        {
            var type = typeof(T);
            if (!type.IsEnum) throw new InvalidOperationException();
            foreach (var field in type.GetFields())
            {
                var attribute = Attribute.GetCustomAttribute(field,
                    typeof(DescriptionAttribute)) as DescriptionAttribute;
                if (attribute != null)
                {
                    if (attribute.Description == description)
                        return (T)field.GetValue(null);
                }
                else
                {
                    if (field.Name == description)
                        return (T)field.GetValue(null);
                }
            }
            throw new ArgumentException("Not found.", "description");
            // or return default(T);
        }
    }

    /// <summary>
    /// тип записи лога
    /// </summary>
    public enum LogRecordType
    {
        InterruptInfoBegin,   // начало записи по прерыванию в логе

        InterruptTime,        // время прерывания
        SystemTemperature,    // температура системы

        InterruptRecordBegin,// начало записи для канала

        ChannelNumber,       // запись номера канала
        RodPosition,         // позиция штанги для канала
        MoveTime,            // время движения штанги для канала
        Motoresource,        // моторесурс канала
        EthalonNumber,       // номер эталона, с которым сравнивали
        CompareResult,       // результат сравнения с эталоном

        InterruptDataBegin,  // начало данных прерывания
        InterruptDataEnd,    // конец данных прерывания

        EthalonDataFollow, // следом идут данные эталона, с которым сравнивали

        InterruptRecordEnd, // конец записи для канада

        InterruptInfoEnd,    // конец записи по прерыванию
        RelayTriggeredTime, // время срабатывания защиты
        ChannelInductiveSensorState, // состояние индуктивного датчика канала

        OscDataFollow, // идут данные по току для канала
        DataArrivedTime, // смещение от начала записей по току до начала данных по прерыванию, миллисекунд
        PreviewCount, // кол-во записей в предпросмотре по току
        RodMoveLength, // длина перемещения штанги, мм
        DirectionData, // данные по изменению направления вращения энкодера

    }

    /// <summary>
    /// тип сравнения с эталоном
    /// </summary>
    public enum EthalonCompareNumber
    {
        [Description("без сравнения")]
        NoEthalon,

        [Description("канал 1, вверх")]
        E1up,
        [Description("канал 1, вниз")]
        E1down,

        [Description("канал 2, вверх")]
        E2up,
        [Description("канал 2, вниз")]
        E2down,

        [Description("канал 3, вверх")]
        E3up,
        [Description("канал 3, вниз")]
        E3down,

    }

    /// <summary>
    /// результат сравнения с эталоном
    /// </summary>
    public enum EthalonCompareResult
    {
        [Description("нет данных")]
        NoSourcePulses, // нет исходных данных в списке

        [Description("нет эталона")]
        NoEthalonFound, // не найдено эталона для канала

        [Description("отказ штанги")]
        RodBroken,      // штанга поломана

        [Description("совпадение")]
        MatchEthalon,    // результат соответствует эталону

        [Description("несовпадение")]
        MismatchEthalon, // результат не соответствует эталону
    }

    /// <summary>
    /// позиция штанги
    /// </summary>
    public enum RodPosition
    {
        [Description("поломана")]
        Broken, // штанга поломана и находится в промежуточной позиции

        [Description("вверху")]
        Up, // в верхней позиции

        [Description("внизу")]
        Down // в нижней позиции
    }
    /// <summary>
    /// информация по прерыванию
    /// </summary>
    public class InterruptInfo
    {
        public DateTime InterruptTime; // время прерывания
        public float SystemTemperature; // температура системы
    }

    /// <summary>
    /// запись по прерыванию
    /// </summary>
    public class InterruptRecord
    {
        public InterruptInfo InterruptInfo; // основная информация
        public byte ChannelNumber; // номер канала
        //DEPRECATED: public InductiveSensorState InductiveSensorState;
        public RodPosition RodPosition; // позиция штанги
        public int MoveTime; // время перемещения
        public int Motoresource; // моторесурс
        public int DataArrivedTime; // время поступления прерывания относительно начала графика по току
        public EthalonCompareNumber EthalonCompareNumber; // номер сравнения с эталоном
        public EthalonCompareResult EthalonCompareResult; // результат сравнения с эталоном
        public List<int> InterruptData = new List<int>(); // данные прерывания
        public List<int> EthalonData = new List<int>(); // данные эталона

        // данные по току каналов
        public List<int> CurrentTimes = new List<int>();
        public List<int> CurrentData1 = new List<int>();
        public List<int> CurrentData2 = new List<int>();
        public List<int> CurrentData3 = new List<int>();

        public List<int> Directions = new List<int>(); // список направлений движения привода

        public int PreviewCount = 0; // кол-во превью по току
        public int RodMoveLength = 50; // величина перемещения штанги
    }

    public delegate void ShowInterruptInfo(InterruptRecord info);

    public enum ConnectionMode
    {
        None,
        Serial,
        TCP
    }

    public enum MBusFunction
    {
        None = 0,     // нет функции
        ListFiles,    // запрошен список файлов в директории
        FileContent,  // запрошено содержимое файла
        DeleteFile,   // запрошено удаление файла
        SetDeviceTime, // запрошена установка времени прибора
    }

    /// <summary>
    /// типы узлов дерева SD
    /// </summary>
    public enum SDNodeTags
    {
        /// <summary>
        /// никакой
        /// </summary>
        TagDummyNode = 100,

        /// <summary>
        /// неинициализированная папка (файлы ещё не загружены)
        /// </summary>
        TagFolderUninitedNode,

        /// <summary>
        /// инициализированная папка (файлы уже загружены)
        /// </summary>
        TagFolderNode,

        /// <summary>
        /// файл
        /// </summary>
        TagFileNode
    }

    public class SDNodeTagHelper
    {
        public SDNodeTags Tag = SDNodeTags.TagFolderUninitedNode;
        public string FileName = "";
        public bool IsDirectory = false;
        public SDNodeTagHelper(SDNodeTags tag, string fileName, bool isDir)
        {
            Tag = tag;
            FileName = fileName;
            IsDirectory = isDir;
        }
    }
}
