﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace UROVModbus
{
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