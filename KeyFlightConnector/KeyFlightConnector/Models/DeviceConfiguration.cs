using System;
using System.Collections.Generic;
using System.Linq;
using Newtonsoft.Json;

namespace KeyFlightConnector.Models
{
    /// <summary>
    /// Complete device configuration including all input modules
    /// </summary>
    public class DeviceConfiguration
    {
        /// <summary>Configuration version</summary>
        [JsonProperty("version")]
        public string Version { get; set; } = "1.0";

        /// <summary>User-defined device name</summary>
        [JsonProperty("deviceName")]
        public string DeviceName { get; set; } = "KeyFlight Controller";

        /// <summary>List of configured input modules</summary>
        [JsonProperty("modules")]
        public List<InputDevice> Modules { get; set; } = new List<InputDevice>();

        /// <summary>Maximum number of modules supported</summary>
        [JsonIgnore]
        public const int MaxModules = 40;

        /// <summary>
        /// Adds a new input module
        /// </summary>
        public bool AddModule(InputDevice module, out string error)
        {
            error = string.Empty;

            // Check module limit
            if (Modules.Count >= MaxModules)
            {
                error = $"Maximum {MaxModules} modules reached";
                return false;
            }

            // Validate module
            if (!module.Validate(out error))
            {
                return false;
            }

            // Check for duplicate ID
            if (Modules.Any(m => m.Id == module.Id))
            {
                error = $"Module ID {module.Id} already exists";
                return false;
            }

            // Check for pin conflicts
            var pinConflict = Modules.FirstOrDefault(m => m.Pin == module.Pin);
            if (pinConflict != null)
            {
                error = $"Pin {module.Pin} already used by {pinConflict.Name}";
                return false;
            }

            Modules.Add(module);
            return true;
        }

        /// <summary>
        /// Removes a module by ID
        /// </summary>
        public bool RemoveModule(int id)
        {
            var module = Modules.FirstOrDefault(m => m.Id == id);
            if (module != null)
            {
                Modules.Remove(module);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Gets the next available module ID
        /// </summary>
        public int GetNextModuleId()
        {
            if (Modules.Count == 0) return 0;
            return Modules.Max(m => m.Id) + 1;
        }

        /// <summary>
        /// Serializes configuration to JSON
        /// </summary>
        public string ToJson()
        {
            return JsonConvert.SerializeObject(this, Formatting.Indented);
        }

        /// <summary>
        /// Deserializes configuration from JSON
        /// </summary>
        public static DeviceConfiguration FromJson(string json)
        {
            return JsonConvert.DeserializeObject<DeviceConfiguration>(json)
                   ?? new DeviceConfiguration();
        }
    }
}
