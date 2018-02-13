// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System;
using System.Collections.Generic;
using System.IO;

namespace CxbxDebugger.Utils
{
    class INIReader
    {
        string[] LineContents;
        List<int> CachedSectionIndexes = new List<int>();

        public INIReader(string FileName)
        {
            LineContents = File.ReadAllLines(FileName);
            CacheSectionIndexes();
        }

        private void CacheSectionIndexes()
        {
            int LineIndex = 0;
            foreach (string Line in LineContents)
            {
                if (Line.StartsWith("[") && Line.EndsWith("]"))
                {
                    CachedSectionIndexes.Add(LineIndex);
                }

                ++LineIndex;
            }
        }

        public bool IsValid
        {
            get
            {
                return CachedSectionIndexes.Count > 0;
            }
        }

        private int FindSectionIndex(string SectionName)
        {
            string SectionString = string.Format("[{0}]", SectionName);
            return CachedSectionIndexes.FindIndex(Line => LineContents[Line] == SectionString);
        }

        private Tuple<int, int> GetSectionExtent(int SectionIndex)
        {
            return new Tuple<int, int>(SectionIndex, SectionIndex + 1);
        }

        private Tuple<int, int> GetContentsExtent(int SectionIndex)
        {
            var SectionExtent = GetSectionExtent(SectionIndex);

            int ContentsStart = CachedSectionIndexes[SectionExtent.Item1] + 1;

            // Default to last line of the file
            int ContentsEnd = LineContents.Length;

            if (SectionExtent.Item2 < CachedSectionIndexes.Count)
            {
                ContentsEnd = CachedSectionIndexes[SectionExtent.Item2];
            }

            return new Tuple<int, int>(ContentsStart, ContentsEnd);
        }

        private Tuple<int, int> GetContentsExtent(string SectionName)
        {
            int Index = FindSectionIndex(SectionName);
            if (Index == -1)
                return null;

            return GetContentsExtent(Index);
        }

        public bool HasSection(string SectionName)
        {
            return FindSectionIndex(SectionName) != -1;
        }

        public string[] GetSectionContents(string SectionName)
        {
            var Extent = GetContentsExtent(SectionName);
            if (Extent == null)
                return null;

            List<string> Sections = new List<string>();

            for (int i = Extent.Item1; i < Extent.Item2; ++i)
            {
                Sections.Add(LineContents[i]);
            }

            return Sections.ToArray();
        }

        public KeyValuePair<string, string>[] GetSectionContentsPairs(string SectionName)
        {
            var Extent = GetContentsExtent(SectionName);
            if (Extent == null)
                return null;

            var KeyValuePairs = new List<KeyValuePair<string, string>>();

            List<string> Sections = new List<string>();

            for (int i = Extent.Item1; i < Extent.Item2; ++i)
            {
                var StringSplit = LineContents[i].Split('=');
                if (StringSplit.Length == 2)
                {
                    KeyValuePairs.Add(new KeyValuePair<string, string>(StringSplit[0], StringSplit[1]));
                }
            }

            return KeyValuePairs.ToArray();
        }

        public string GetKeyValue(string SectionName, string Key)
        {
            var Extent = GetContentsExtent(SectionName);
            if (Extent == null)
                return null;

            string KeyString = string.Format("{0}=", Key);

            for (int i = Extent.Item1; i < Extent.Item2; ++i)
            {
                if (LineContents[i].StartsWith(KeyString))
                {
                    return LineContents[i].Substring(KeyString.Length);
                }
            }

            return null;
        }
    }
}
