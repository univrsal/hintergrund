from .rule import Rule
import requests
from datetime import datetime, timedelta
from typing import Optional, Dict, Any
import logging

logger = logging.getLogger(__name__)

class WeatherRule(Rule):
    """Rule that applies based on current weather conditions from yr.no API.
    
    Uses the yr.no weather API to check current weather conditions at a specific location.
    The rule is active if the current weather matches the specified condition.
    """

    latitude: float
    longitude: float
    weather_condition: str  # e.g., "rain", "snow", "clear", "cloudy", "thunderstorm"
    cache_duration: int = 600  # Cache weather data for 10 minutes by default
    
    # Class-level cache to avoid making too many API requests
    _weather_cache: Dict[str, Dict[str, Any]] = {}
    
    def __init__(self, **data):
        super().__init__(**data)
        
    def _get_cache_key(self) -> str:
        """Return a unique cache key for this rule based on latitude and longitude."""
        return f"{self.latitude},{self.longitude}"
    
    def _get_weather_data(self) -> Optional[Dict[str, Any]]:
        """Fetch weather data from yr.no API with caching."""
        now = datetime.now()
        
        # Check if we have cached data that's still valid
        if self._get_cache_key() in self._weather_cache:
            cached_data = self._weather_cache[self._get_cache_key()]
            cache_time = cached_data.get('timestamp', datetime.min)
            if now - cache_time < timedelta(seconds=self.cache_duration):
                return cached_data.get('data')
        
        try:
            # yr.no API endpoint for location forecast
            url = f"https://api.met.no/weatherapi/locationforecast/2.0/compact"
            params = {
                'lat': self.latitude,
                'lon': self.longitude
            }
            headers = {
                'User-Agent': 'hintergrund-wallpaper-app/1.0 (https://github.com/user/hintergrund)'
            }
            
            response = requests.get(url, params=params, headers=headers, timeout=10)
            response.raise_for_status()
            
            data = response.json()
            
            # Cache the response
            self._weather_cache[self._get_cache_key()] = {
                'timestamp': now,
                'data': data
            }
            
            return data
            
        except Exception as e:
            logger.error(f"Failed to fetch weather data: {e}")
            return None
    
    def _get_current_weather_symbol(self, weather_data: Dict[str, Any]) -> Optional[str]:
        """Extract current weather symbol from yr.no API response."""
        try:
            # Get the current time series data (first entry should be current/next hour)
            timeseries = weather_data.get('properties', {}).get('timeseries', [])
            if not timeseries:
                return None
            
            # Get the first entry (current weather)
            current = timeseries[0]
            symbol_code = current.get('data', {}).get('next_1_hours', {}).get('summary', {}).get('symbol_code')
            
            # If next_1_hours is not available, try next_6_hours
            if not symbol_code:
                symbol_code = current.get('data', {}).get('next_6_hours', {}).get('summary', {}).get('symbol_code')
            
            return symbol_code
            
        except Exception as e:
            logger.error(f"Failed to parse weather symbol: {e}")
            return None
    
    def _matches_condition(self, symbol_code: str) -> bool:
        """Check if the weather symbol matches our condition."""
        if not symbol_code:
            return False
        
        # Convert symbol code to lowercase for comparison
        symbol_lower = symbol_code.lower()
        condition_lower = self.weather_condition.lower()
        
        # Map weather conditions to yr.no symbol patterns
        condition_patterns = {
            'clear': ['clearsky', 'fair'],
            'cloudy': ['partlycloudy', 'cloudy'],
            'rain': ['lightrain', 'rain', 'heavyrain', 'rainshowers'],
            'snow': ['lightsnow', 'snow', 'heavysnow', 'snowshowers'],
            'thunderstorm': ['lightrainthunder', 'rainthunder', 'heavyrainthunder', 
                           'lightsnowthunder', 'snowthunder', 'heavysnowthunder'],
            'fog': ['fog'],
            'sleet': ['lightsleet', 'sleet', 'heavysleet', 'sleetshowers']
        }
        
        # Check if condition matches any pattern
        patterns = condition_patterns.get(condition_lower, [condition_lower])
        
        for pattern in patterns:
            if pattern in symbol_lower:
                return True
        
        # Also allow direct symbol code matching
        return condition_lower in symbol_lower
    
    def is_active(self) -> bool:
        """Check if the rule is active based on current weather conditions."""
        if not self.enabled:
            return False
        
        weather_data = self._get_weather_data()
        if not weather_data:
            logger.warning(f"No weather data available for {self.latitude}, {self.longitude}")
            return False
        
        symbol_code = self._get_current_weather_symbol(weather_data)
        if not symbol_code:
            logger.warning("No weather symbol found in API response")
            return False
        
        matches = self._matches_condition(symbol_code)
        
        if matches:
            logger.info(f"Weather rule '{self.name}' active: {symbol_code} matches '{self.weather_condition}'")
        
        return matches
