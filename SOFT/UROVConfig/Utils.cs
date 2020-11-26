﻿using System;
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

namespace UROVConfig
{

    public delegate void ShowInterruptInfo(InterruptRecord info);

    public enum TreeNodeType
    {
        MainSettingsNode,
        SDSettingsNode,
        AdditionalSettingsNode
    }

    public enum AnswerBehaviour
    {
        Normal,
        SDCommandLS,
        SDCommandFILE,
    }

    public enum SDNodeTags
    {
        TagDummyNode = 100,
        TagFolderUninitedNode,
        TagFolderNode,
        TagFileNode
    }

    /*
    public enum FileDownloadFlags
    {
        View,
        DownloadEthalon
    }
    */

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

    }

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

    public enum RodPosition
    {
        [Description("поломана")]
        Broken, // штанга поломана и находится в промежуточной позиции

        [Description("вверху")]
        Up, // в верхней позиции

        [Description("внизу")]
        Down // в нижней позиции
    }

    public enum InductiveSensorState
    {
        [Description("неисправен")]
        Broken,
        [Description("исправен")]
        Good
    }

    public class InterruptInfo
    {
        public DateTime InterruptTime;
        public float SystemTemperature;
    }

    public class ChannelData
    {
        public int RecordTime = 0;
        public List<int> Values = new List<int>();       

    };

    public class CurrentInfo
    {
        public int Time;
        public int Channel1;
        public int Channel2;
        public int Channel3;
    }

    public class InterruptRecord
    {
        public InterruptInfo InterruptInfo;
        public byte ChannelNumber;
        //DEPRECATED: public InductiveSensorState InductiveSensorState;
        public RodPosition RodPosition;
        public int MoveTime;
        public int Motoresource;
        public int DataArrivedTime;
        public EthalonCompareNumber EthalonCompareNumber;
        public EthalonCompareResult EthalonCompareResult;
        public List<int> InterruptData = new List<int>();
        public List<int> EthalonData = new List<int>();

        // данные по току каналов
        public List<int> CurrentTimes = new List<int>();
        public List<int> CurrentData1 = new List<int>();
        public List<int> CurrentData2 = new List<int>();
        public List<int> CurrentData3 = new List<int>();

        public int PreviewCount = 0;
        public int RodMoveLength = 50;
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

    public class MyEnumConverter : EnumConverter
    {
        private Type type;

        public MyEnumConverter(Type type)
            : base(type)
        {
            this.type = type;
        }


        public override object ConvertTo(ITypeDescriptorContext context,
            CultureInfo culture, object value, Type destType)
        {
            try
            {
                FieldInfo fi = type.GetField(Enum.GetName(type, value));
                DescriptionAttribute descAttr =
                  (DescriptionAttribute)Attribute.GetCustomAttribute(
                    fi, typeof(DescriptionAttribute));

                if (descAttr != null)
                    return descAttr.Description;
                else
                    return value.ToString();
            }
            catch
            {
                return Enum.GetValues(type).GetValue(0);
            }
        }

        public override object ConvertFrom(ITypeDescriptorContext context,
            CultureInfo culture, object value)
        {
            foreach (FieldInfo fi in type.GetFields())
            {
                DescriptionAttribute descAttr =
                  (DescriptionAttribute)Attribute.GetCustomAttribute(
                    fi, typeof(DescriptionAttribute));

                if ((descAttr != null) && ((string)value == descAttr.Description))
                    return Enum.Parse(type, fi.Name);
            }
            return Enum.Parse(type, (string)value);
        }
    }

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

    class YesNoConverter : BooleanConverter
    {
        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            if (value is bool && destinationType == typeof(string))
            {
                return values[(bool)value ? 1 : 0];
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            string txt = value as string;
            if (values[0] == txt) return false;
            if (values[1] == txt) return true;
            return base.ConvertFrom(context, culture, value);
        }

        private string[] values = new string[] { "Нет", "Да" };
    }

    /// <summary>
    /// Range modification for direct edit override
    /// </summary>
    public class NumericUpDownTypeConverter : TypeConverter
    {
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            // Attempt to do them all
            return true;
        }


        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value)
        {
            try
            {
                string Value;
                if (!(value is string))
                {
                    Value = Convert.ChangeType(value, context.PropertyDescriptor.PropertyType).ToString();
                }
                else
                    Value = value as string;
                decimal decVal;
                if (!decimal.TryParse(Value, out decVal))
                    decVal = decimal.One;
                MinMaxAttribute attr = (MinMaxAttribute)context.PropertyDescriptor.Attributes[typeof(MinMaxAttribute)];
                if (attr != null)
                {
                    decVal = attr.PutInRange(decVal);
                }
                return Convert.ChangeType(decVal, context.PropertyDescriptor.PropertyType);
            }
            catch
            {
                return base.ConvertFrom(context, culture, value);
            }
        }

        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType)
        {
            try
            {
                return destinationType == typeof(string)
                   ? Convert.ChangeType(value, context.PropertyDescriptor.PropertyType).ToString()
                   : Convert.ChangeType(value, destinationType);
            }
            catch { }
            return base.ConvertTo(context, culture, value, destinationType);
        }
    }

    // ReSharper disable MemberCanBePrivate.Global
    /// <summary>
    /// Attribute to allow ranges to be added to the numeric updowner
    /// </summary>
    [AttributeUsage(AttributeTargets.Property, AllowMultiple = false)]
    public class MinMaxAttribute : Attribute
    {
        public decimal Min { get; private set; }
        public decimal Max { get; private set; }
        public decimal Increment { get; private set; }
        public int DecimalPlaces { get; private set; }

        /// <summary>
        /// Use to make a simple UInt16 max. Starts at 0, increment = 1
        /// </summary>
        /// <param name="max"></param>
        public MinMaxAttribute(UInt16 max)
           : this((decimal)UInt16.MinValue, max)
        {
        }

        /// <summary>
        /// Use to make a simple integer (or default conversion) based range.
        /// default inclrement is 1
        /// </summary>
        /// <param name="min"></param>
        /// <param name="max"></param>
        /// <param name="increment"></param>
        public MinMaxAttribute(int min, int max, int increment = 1)
           : this((decimal)min, max, increment)
        {
        }

        /// <summary>
        /// Set the Min, Max, increment, and decimal places to be used.
        /// </summary>
        /// <param name="min"></param>
        /// <param name="max"></param>
        /// <param name="increment"></param>
        /// <param name="decimalPlaces"></param>
        public MinMaxAttribute(decimal min, decimal max, decimal increment = decimal.One, int decimalPlaces = 0)
        {
            Min = min;
            Max = max;
            Increment = increment;
            DecimalPlaces = decimalPlaces;
        }

        /// <summary>
        /// Validation function to check if the value is withtin the range (inclusive)
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public bool IsInRange(object value)
        {
            decimal checkedValue = (decimal)Convert.ChangeType(value, typeof(decimal));
            return ((checkedValue <= Max)
               && (checkedValue >= Min)
               );
        }

        /// <summary>
        /// Takes the value and adjusts if it is out of bounds.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public decimal PutInRange(object value)
        {
            decimal checkedValue = (decimal)Convert.ChangeType(value, typeof(decimal));
            if (checkedValue > Max)
                checkedValue = Max;
            else if (checkedValue < Min)
                checkedValue = Min;
            return checkedValue;
        }
    }
    // ReSharper restore MemberCanBePrivate.Global


    public class NumericUpDownTypeEditor : UITypeEditor
    {
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            if (context == null || context.Instance == null)
                return base.GetEditStyle(context);
            return context.PropertyDescriptor.IsReadOnly ? UITypeEditorEditStyle.None : UITypeEditorEditStyle.DropDown;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            try
            {
                if (context == null || context.Instance == null || provider == null)
                    return value;

                //use IWindowsFormsEditorService object to display a control in the dropdown area  
                IWindowsFormsEditorService frmsvr = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));
                if (frmsvr == null)
                    return value;

                MinMaxAttribute attr = (MinMaxAttribute)context.PropertyDescriptor.Attributes[typeof(MinMaxAttribute)];
                if (attr != null)
                {
                    NumericUpDown nmr = new NumericUpDown
                    {
                        Size = new Size(60, 120),
                        Minimum = attr.Min,
                        Maximum = attr.Max,
                        Increment = attr.Increment,
                        DecimalPlaces = attr.DecimalPlaces,
                        Value = attr.PutInRange(value)
                    };
                    frmsvr.DropDownControl(nmr);
                    context.OnComponentChanged();
                    return Convert.ChangeType(nmr.Value, context.PropertyDescriptor.PropertyType);
                }
            }
            catch { }
            return value;
        }
    }






    public class CsvConverter : TypeConverter
    {
        // Overrides the ConvertTo method of TypeConverter.
        public override object ConvertTo(ITypeDescriptorContext context,
           CultureInfo culture, object value, Type destinationType)
        {
            List<String> v = value as List<String>;
            if (destinationType == typeof(string))
            {
                return String.Join(",", v.ToArray());
            }
            return base.ConvertTo(context, culture, value, destinationType);
        }
    }


 /* 

    public class ConfigGeneralSettings
    {

        
        [Browsable(true)]
        [Category("1. Моторесурс текущий")]
        [Description("Текущий моторесурс канала №1.")]
        [DisplayName("Канал №1")]
        [TypeConverter(typeof(NumericUpDownTypeConverter))]
        [Editor(typeof(NumericUpDownTypeEditor), typeof(UITypeEditor)), MinMaxAttribute(0, 100000000)]
        public int MotoresourceCurrent1 { get { return Config.Instance.MotoresourceCurrent1; } set { Config.Instance.MotoresourceCurrent1 = value; } }
        public bool ShouldSerializeMotoresourceCurrent1() { return false; }

        [Browsable(true)]
        [Category("2. Информация")]
        [Description("Количество свободной памяти, байт.")]
        [DisplayName("Память")]
        [ReadOnly(true)]
        public int FreeRAM { get { return Config.Instance.FreeRAM; }  }
        public bool ShouldSerializeFreeRAM() { return false; }


        [Browsable(true)]
        [Category("2. Информация")]
        [Description("Версия ядра, используемого прошивкой.")]
        [DisplayName("Версия ядра")]
        [ReadOnly(true)]
        public string CoreVersion { get { return Config.Instance.CoreVersion; } }
        public bool ShouldSerializeCoreVersion() { return false; }

        [Browsable(true)]
        [Category("2. Информация")]
        [Description("Время, установленное на контроллере.")]
        [DisplayName("Время")]
        [ReadOnly(true)]
        public string ControllerDateTime
        {
            get
            {
                if (Config.Instance.ControllerDateTime.Length < 1)
                    return "-";

                return Config.Instance.ControllerDateTime;
            }

        }
        public bool ShouldSerializeControllerDateTime() { return false; }



    }
*/
    public class FeaturesSettings
    {
        private bool sdAvailable = true;
        public bool SDAvailable { get { return sdAvailable; } }


        public void Clear()
        {
            sdAvailable = true;
        }

    }

    public class ControllerNameItem
    {
        [XmlAttribute]
        public string id;
        [XmlAttribute]
        public string value;
    }

    public class ControllerNames
    {
        private static object lockFlag = new object();
        private static ControllerNames instance;

        private Dictionary<string, string> cNames = new Dictionary<string, string>();
        public Dictionary<string, string> Names
        {
            get { return cNames; }
            set { cNames = value; }
        }

        public static ControllerNames Instance
        {
            get
            {
                lock (lockFlag)
                {
                    if (instance == null)
                    {

                        try
                        {
                            //Пытаемся загрузить файл с диска и десериализовать его
                            using (System.IO.FileStream fs =
                                new System.IO.FileStream(Application.StartupPath
                                + "\\controllers.xml", System.IO.FileMode.Open))
                            {
                                System.Xml.Serialization.XmlSerializer xs =
                                    new System.Xml.Serialization.XmlSerializer(typeof(ControllerNameItem[]), new XmlRootAttribute() { ElementName = "items" });

                                instance = new ControllerNames();

                                //instance = (ControllerNames)xs.Deserialize(fs);
                                instance.cNames = ((ControllerNameItem[])xs.Deserialize(fs)).ToDictionary(i => i.id, i => i.value);


                            }
                        }
                        catch (Exception)
                        {
                            //Если не удалось десериализовать то просто создаем новый экземпляр
                            instance = new ControllerNames();
                        }

                    } // if instance == null
                } // lock
                return instance;
            } // get

        }

        public void Save()
        {
            using (System.IO.FileStream fs =
              new System.IO.FileStream(Application.StartupPath + "\\controllers.xml", System.IO.FileMode.Create))
            {
                System.Xml.Serialization.XmlSerializer xs =
                    new System.Xml.Serialization.XmlSerializer(typeof(ControllerNameItem[]), new XmlRootAttribute() { ElementName = "items" });

                xs.Serialize(fs, cNames.Select(kv => new ControllerNameItem() { id = kv.Key, value = kv.Value }).ToArray());

            }
        }

        public static void Reload()
        {
            instance = null;
        }


        private ControllerNames()
        {
        }
    }

    public class ArchiveTreeRootItem
    {
        private string guid = "";
        public string GUID
        {
            get { return this.guid; }
            set { this.guid = value; }
        }

        public ArchiveTreeRootItem(string g)
        {
            guid = g;
        }

        public override string ToString()
        {
            if(ControllerNames.Instance.Names.ContainsKey(this.guid))
            {
                return ControllerNames.Instance.Names[this.guid];
            }
            return this.guid;
        }
    }

    public class ArchiveTreeLogItem
    {
        private ArchiveTreeRootItem parent = null;
        public ArchiveTreeRootItem Parent { get { return this.parent; } }

        public ArchiveTreeLogItem(ArchiveTreeRootItem p)
        {
            parent = p;
        }
    }

    public class ArchiveTreeLogItemRecord
    {
        private ArchiveTreeLogItem parent = null;
        public ArchiveTreeLogItem Parent { get { return this.parent; } }

        private string filename = "";
        public string FileName { get { return filename; } }

        public ArchiveTreeLogItemRecord(ArchiveTreeLogItem p, string fname)
        {
            parent = p;
            filename = fname;
        }
        
    }

    public class ArchiveTreeEthalonItem
    {
        private ArchiveTreeRootItem parent = null;
        public ArchiveTreeRootItem Parent { get { return this.parent; } }

        public ArchiveTreeEthalonItem(ArchiveTreeRootItem p)
        {
            parent = p;
        }
    }

    public class ArchiveTreeEthalonItemRecord
    {
        private ArchiveTreeEthalonItem parent = null;
        public ArchiveTreeEthalonItem Parent { get { return this.parent; } }

        private string filename = "";
        public string FileName { get { return filename; } }

        public ArchiveTreeEthalonItemRecord(ArchiveTreeEthalonItem p, string fname)
        {
            parent = p;
            filename = fname;
        }

    }

    public class ArchiveSettings
    {
        public ArchiveSettings()
        {

        }

        private int motoresourceMax1 = 0;
        private int motoresourceMax2 = 0;
        private int motoresourceMax3 = 0;

        [XmlElement("mmax1")]
        public int MotoresourceMax1 { get { return motoresourceMax1; } set { motoresourceMax1 = value; } }
        [XmlElement("mmax2")]
        public int MotoresourceMax2 { get { return motoresourceMax2; } set { motoresourceMax2 = value; } }
        [XmlElement("mmax3")]
        public int MotoresourceMax3 { get { return motoresourceMax3; } set { motoresourceMax3 = value; } }

        private int pulses1 = 0;
        private int pulses2 = 0;
        private int pulses3 = 0;

        [XmlElement("pulses1")]
        public int Pulses1 { get { return pulses1; } set { pulses1 = value; } }
        [XmlElement("pulses2")]
        public int Pulses2 { get { return pulses2; } set { pulses2 = value; } }
        [XmlElement("pulses3")]
        public int Pulses3 { get { return pulses3; } set { pulses3 = value; } }

        private int lowBorder = 0;
        private int highBorder = 0;

        [XmlElement("lowBorder")]
        public int LowBorder { get { return lowBorder; } set { lowBorder = value; } }
        [XmlElement("highBorder")]
        public int HighBorder { get { return highBorder; } set { highBorder = value; } }

        private int delta1 = 0;
        private int delta2 = 0;
        private int delta3 = 0;

        [XmlElement("delta1")]
        public int Delta1 { get { return delta1; } set { delta1 = value; } }
        [XmlElement("delta2")]
        public int Delta2 { get { return delta2; } set { delta2 = value; } }
        [XmlElement("delta3")]
        public int Delta3 { get { return delta3; } set { delta3 = value; } }

        private int rodMoveLength = 50;
        [XmlElement("rodMoveTime")]
        public int RodMoveLength { get { return rodMoveLength; } set { rodMoveLength = value; } }

        public void ApplyFromConfig()
        {
            Config c = Config.Instance;

            this.motoresourceMax1 = c.MotoresourceMax1;
            this.rodMoveLength = c.RodMoveLength;
            //DEPRECATED: this.motoresourceMax2 = c.MotoresourceMax2;
            //DEPRECATED: this.motoresourceMax3 = c.MotoresourceMax3;

            this.lowBorder = c.LowBorder;
            this.highBorder = c.HighBorder;

            this.pulses1 = c.Pulses1;
            //DEPRECATED: this.pulses2 = c.Pulses2;
            //DEPRECATED: this.pulses3 = c.Pulses3;

            this.delta1 = c.Delta1;
            //DEPRECATED: this.delta2 = c.Delta2;
            //DEPRECATED: this.delta3 = c.Delta3;

        }

        public static ArchiveSettings Load(string filename)
        {
            ArchiveSettings toLoad = null;
            try
            {
                //Пытаемся загрузить файл с диска и десериализовать его
                using (FileStream fs =
                    new FileStream(filename, FileMode.Open))
                {
                    System.Xml.Serialization.XmlSerializer xs =
                        new System.Xml.Serialization.XmlSerializer(typeof(ArchiveSettings));
                    toLoad = (ArchiveSettings)xs.Deserialize(fs);


                }
            }
            catch (Exception)
            {
                //Если не удалось десериализовать то просто создаем новый экземпляр
                toLoad = new ArchiveSettings();
            }
            return toLoad;
        }

        public bool Save(string filename)
        {
            bool result = true;
            try
            {

                using (FileStream fs =
                  new FileStream(filename, FileMode.Create))
                {
                    System.Xml.Serialization.XmlSerializer xs =
                        new System.Xml.Serialization.XmlSerializer(typeof(ArchiveSettings));
                    xs.Serialize(fs, this);
                }
            }
            catch (Exception)
            {
                result = false;
            }
            return result;
        }

    }

    public class Config
    {


        private static object lockFlag = new object();
        private static Config instance;

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

        public static void Reload()
        {
            instance = null;
        }

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

        public void Clear()
        {
            motoresourceCurrent1 = 0;
            //DEPRECATED: motoresourceCurrent2 = 0;
            //DEPRECATED: motoresourceCurrent3 = 0;

            motoresourceMax1 = 0;
            //DEPRECATED: motoresourceMax2 = 0;
            //DEPRECATED: motoresourceMax3 = 0;

            pulses1 = 0;
            //DEPRECATED: pulses2 = 0;
            //DEPRECATED: pulses3 = 0;

            delta1 = 0;
            //DEPRECATED: delta2 = 0;
            //DEPRECATED: delta3 = 0;

            lowBorder = 0;
            highBorder = 0;

            controllerGUID = "";

            relayDelay = 0;
            acsDelay = 0;

            skipCounter = 1;

            currentCoeff = 1;
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


        private int motoresourceCurrent1 = 0;
        //DEPRECATED: private int motoresourceCurrent2 = 0;
        //DEPRECATED: private int motoresourceCurrent3 = 0;

        [XmlIgnore]
        public int MotoresourceCurrent1 { get { return motoresourceCurrent1; } set { motoresourceCurrent1 = value; } }
        //DEPRECATED: public int MotoresourceCurrent2 { get { return motoresourceCurrent2; } set { motoresourceCurrent2 = value; } }
        //DEPRECATED: public int MotoresourceCurrent3 { get { return motoresourceCurrent3; } set { motoresourceCurrent3 = value; } }

        private int motoresourceMax1 = 0;
        //DEPRECATED: private int motoresourceMax2 = 0;
        //DEPRECATED: private int motoresourceMax3 = 0;

        [XmlIgnore]
        public int MotoresourceMax1 { get { return motoresourceMax1; } set { motoresourceMax1 = value; } }
        //DEPRECATED: public int MotoresourceMax2 { get { return motoresourceMax2; } set { motoresourceMax2 = value; } }
        //DEPRECATED: public int MotoresourceMax3 { get { return motoresourceMax3; } set { motoresourceMax3 = value; } }

        private int pulses1 = 0;
        //DEPRECATED: private int pulses2 = 0;
        //DEPRECATED: private int pulses3 = 0;

        [XmlIgnore]
        public int Pulses1 { get { return pulses1; } set { pulses1 = value; } }
        //DEPRECATED: public int Pulses2 { get { return pulses2; } set { pulses2 = value; } }
        //DEPRECATED: public int Pulses3 { get { return pulses3; } set { pulses3 = value; } }

        private int delta1 = 0;
        //DEPRECATED: private int delta2 = 0;
        //DEPRECATED: private int delta3 = 0;

        [XmlIgnore]
        public int Delta1 { get { return delta1; } set { delta1 = value; } }
        //DEPRECATED: public int Delta2 { get { return delta2; } set { delta2 = value; } }
        //DEPRECATED: public int Delta3 { get { return delta3; } set { delta3 = value; } }

        private int lowBorder = 0;
        private int highBorder = 0;

        [XmlIgnore]
        public int LowBorder { get { return lowBorder; } set { lowBorder = value; } }
        [XmlIgnore]
        public int HighBorder { get { return highBorder; } set { highBorder = value; } }

        private int relayDelay = 0;
        [XmlIgnore]
        public int RelayDelay { get { return relayDelay; } set { relayDelay = value; } }

        private int currentCoeff = 0;
        [XmlIgnore]
        public int CurrentCoeff { get { return currentCoeff; } set { currentCoeff = value; } }

        private int acsDelay = 0;
        [XmlIgnore]
        public int ACSDelay { get { return acsDelay; } set { acsDelay = value; } }

        private int maxIdleTime = 0;
        [XmlIgnore]
        public int MaxIdleTime { get { return maxIdleTime; } set { maxIdleTime = value; } }

        private int rodMoveLength = 50;
        [XmlIgnore]
        public int RodMoveLength { get { return rodMoveLength; } set { rodMoveLength = value; } }


        private string controllerGUID = "";
        [XmlIgnore]
        public string ControllerGUID { get { return controllerGUID; } set { controllerGUID = value; } }


    }



}
