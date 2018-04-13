// Written by x1nixmzeng for the Cxbx-Reloaded project
//

using System.Collections.Generic;
using System.Xml;

namespace CxbxDebugger
{
    namespace CheatEngine
    {
        public enum CTReaderResult
        {
            Success,
            LoadFailed,
            MissingNode,
            UnsupportedVersion,
        }

        public class CheatTableReader
        {
            CheatTable CTRef;

            public CTReaderResult Load(CheatTable ct, string filename)
            {
                XmlDocument doc = new XmlDocument();

                try
                {
                    doc.Load(filename);
                }
                catch
                {
                    return CTReaderResult.LoadFailed;
                }

                return ParseDocument(doc);
            }

            private CTReaderResult ParseDocument(XmlNode root)
            {
                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                if (xnode.Name == "CheatTable")
                                {
                                    return ParseCheatTable(xnode);
                                }
                                break;
                            }
                    }
                }

                return CTReaderResult.MissingNode;
            }

            private CTReaderResult ParseCheatTable(XmlNode root)
            {
                foreach (XmlAttribute attr in root.Attributes)
                {
                    if (attr.Name == "CheatEngineTableVersion")
                    {
                        if (attr.Value != "21")
                        {
                            return CTReaderResult.UnsupportedVersion;
                        }
                    }
                }

                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                switch (xnode.Name)
                                {
                                    case "CheatEntries":
                                        {
                                            ParseCheatEntries(xnode);
                                            break;
                                        }

                                    case "CheatCodes":
                                        {
                                            ParseCheatCodes(xnode);
                                            break;
                                        }
                                }

                                break;
                            }
                    }
                }

                return CTReaderResult.Success;
            }

            private void ParseCheatEntries(XmlNode root)
            {
                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                if (xnode.Name == "CheatEntry")
                                {
                                    ParseCheatEntry(xnode);
                                }

                                break;
                            }
                    }
                }
            }

            private void ParseCheatCodes(XmlNode root)
            {
                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                if (xnode.Name == "CodeEntry")
                                {
                                    ParseCodeEntry(xnode);
                                }
                                break;
                            }
                    }
                }
            }


            private static uint GetHexNumber(string numeric_string)
            {
                uint swap = 0;
                uint.TryParse(numeric_string, System.Globalization.NumberStyles.HexNumber, null, out swap);
                return swap;
            }

            private static uint GetNumber(string numeric_string)
            {
                uint swap = 0;
                uint.TryParse(numeric_string, System.Globalization.NumberStyles.Integer, null, out swap);
                return swap;
            }

            private void ParseCheatEntry(XmlNode root)
            {
                CheatEntry Entry = new CheatEntry();

                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                switch (xnode.Name)
                                {
                                    case "ID":
                                        Entry.ID = xnode.InnerText;
                                        break;

                                    case "Description":
                                        Entry.Description = xnode.InnerText;
                                        break;

                                    case "LastState":

                                        foreach (XmlAttribute attr in xnode.Attributes)
                                        {
                                            switch (attr.Name)
                                            {
                                                case "Value":
                                                    Entry.LastState.Value = GetNumber(attr.Value);
                                                    break;

                                                case "Activated":
                                                    Entry.LastState.Activated = (GetNumber(attr.Value) != 0);
                                                    break;

                                                case "RealAddress":
                                                    Entry.LastState.RealAddress = GetHexNumber(attr.Value);
                                                    break;
                                            }
                                        }

                                        break;

                                    case "Color":
                                        Entry.Color = GetHexNumber(xnode.InnerText);
                                        break;

                                    case "VariableType":

                                        switch (xnode.InnerText)
                                        {
                                            case "4 Bytes":
                                                Entry.VariableType = Variable.Bytes4;
                                                break;

                                            default:
                                                Entry.VariableType = Variable.Unknown;
                                                break;
                                        }

                                        break;

                                    case "Address":
                                        Entry.Address = xnode.InnerText;
                                        break;
                                }

                                break;
                            }
                    }
                }

                CTRef.CheatEntries.Add(Entry);
            }

            private void ParseCodeEntry(XmlNode root)
            {
                CodeEntry Entry = new CodeEntry();

                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                switch (xnode.Name)
                                {
                                    case "Description":
                                        Entry.Description = xnode.InnerText;
                                        break;

                                    case "Address":
                                        Entry.Address = GetHexNumber(xnode.InnerText);
                                        break;

                                    case "ModuleName":
                                        Entry.ModuleName = xnode.InnerText;
                                        break;

                                    case "ModuleNameOffset":
                                        Entry.ModuleNameOffset = GetHexNumber(xnode.InnerText);
                                        break;

                                    case "Before":
                                        Entry.Before = ParseBytes(xnode);
                                        break;

                                    case "Actual":
                                        Entry.Actual = ParseBytes(xnode);
                                        break;

                                    case "After":
                                        Entry.After = ParseBytes(xnode);
                                        break;
                                }

                                break;
                            }
                    }
                }

                CTRef.CodeList.Add(Entry);
            }

            private byte[] ParseBytes(XmlNode root)
            {
                List<byte> Bytes = new List<byte>();

                foreach (XmlNode xnode in root.ChildNodes)
                {
                    switch (xnode.NodeType)
                    {
                        case XmlNodeType.Element:
                            {
                                if (xnode.Name == "Byte")
                                {
                                    uint Value = GetHexNumber(xnode.InnerText);
                                    Bytes.Add((byte)(Value & 0xFF));
                                }
                            }
                            break;
                    }
                }

                return Bytes.ToArray();
            }
        }
    }
}
