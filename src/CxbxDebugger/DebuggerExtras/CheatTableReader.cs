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
            public static CheatTable FromFile(string filename)
            {
                CheatTable ct = new CheatTable();

                var reader = new CheatTableReader();
                if( reader.Load(ct, filename) == CTReaderResult.Success )
                {
                    return ct;
                }

                return null;
            }

            CheatTable CTRef;

            private CTReaderResult Load(CheatTable ct, string filename)
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

                CTRef = ct;
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
                        uint tableVersion = GetNumber(attr.Value);

                        if (tableVersion < 21)
                        {
                            return CTReaderResult.UnsupportedVersion;
                        }

                        if( tableVersion > 26 ) // 6.7
                        {
                            // warn?
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

                                    case "UserdefinedSymbols":
                                        {
                                            // stub
                                            break;
                                        }

                                    case "Comments":
                                        {
                                            CTRef.Comments = xnode.InnerText;
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

                // Disable by default
                Entry.LastState.Activated = false;

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
                                        Entry.Description = xnode.InnerText.Trim(new char[]{ '\"' });
                                        break;

                                    case "Options":
                                        // stub
                                        // attributes: moHideChildren (bool)
                                        break;

                                    case "LastState":

                                        foreach (XmlAttribute attr in xnode.Attributes)
                                        {
                                            switch (attr.Name)
                                            {
                                                case "Value":
                                                    Entry.LastState.Value = attr.Value;
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
                                        {
                                            uint color = GetHexNumber(xnode.InnerText);
                                            Entry.Color = (color & 0xFFFFFF);
                                        break;
                                        }

                                    case "GroupHeader":
                                        // stub
                                        break;

                                    case "VariableType":

                                        switch (xnode.InnerText)
                                        {
                                            case "Byte":
                                                Entry.VariableType = Variable.Byte;
                                                break;

                                            case "2 Bytes":
                                                Entry.VariableType = Variable.Bytes2;
                                                break;

                                            case "4 Bytes":
                                                Entry.VariableType = Variable.Bytes4;
                                                break;

                                            case "Float":
                                                Entry.VariableType = Variable.Float;
                                                break;

                                            case "Auto Assembler Script":
                                                Entry.VariableType = Variable.Unsupported;
                                                break;

                                            default:
                                                Entry.VariableType = Variable.Unsupported;
                                                break;
                                        }

                                        break;

                                    case "AssemblerScript":
                                        // stub, unsupported
                                        break;

                                    case "Address":
                                        Entry.Address = xnode.InnerText;
                                        break;

                                    case "Hotkeys":
                                        // stub, unsupported
                                        // Hotkey children, Action, Keys->Key children, Value, ID
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

                CTRef.CodeEntires.Add(Entry);
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
