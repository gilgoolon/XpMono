import React, { useState, useEffect } from 'react';
import {
  Box, Typography, Paper, Grid, Card, CardContent,
  IconButton, Collapse, List, ListItem, ListItemText,
  TextField, Button, Alert, Snackbar, Dialog, DialogContent,
  Tabs, Tab, MenuItem
} from '@mui/material';
import { LoadingButton } from '@mui/lab';
import ExpandMoreIcon from '@mui/icons-material/ExpandMore';
import ExpandLessIcon from '@mui/icons-material/ExpandLess';
import SendIcon from '@mui/icons-material/Send';
import ProductViewer from './ProductViewer';
import CommandTemplates from './CommandTemplates';
import axios from 'axios';

import { API_BASE_URL } from '../Config.js'; 

export default function ClientDetails({ client, onSendCommand }) {
  const [commandData, setCommandData] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState(null);
  const [selectedProduct, setSelectedProduct] = useState(null);
  const [ipHistoryExpanded, setIpHistoryExpanded] = useState(false);
  const [activeTab, setActiveTab] = useState(0);
  const [variables, setVariables] = useState({});
  const [variableTypes, setVariableTypes] = useState({});
  const [files, setFiles] = useState([]);
  const [fileLoading, setFileLoading] = useState({});

  useEffect(() => {
    const fetchFiles = async () => {
      try {
        const response = await axios.get(`${API_BASE_URL}/api/files`);
        setFiles(response.data);
      } catch (error) {
        console.error('Failed to fetch files:', error);
      }
    };
    fetchFiles();
  }, []);

  useEffect(() => {
    const extractedVariables = {};
    const extractedTypes = {};
    const regex = /{{[\s]*(\w+)[\s]*}}/g;
    let match;
    while ((match = regex.exec(commandData)) !== null) {
      extractedVariables[match[1]] = variables[match[1]] || '';
      extractedTypes[match[1]] = variableTypes[match[1]] || 'string';
    }
    setVariables(extractedVariables);
    setVariableTypes(extractedTypes);
  }, [commandData]);

  const handleVariableChange = (name, value) => {
    setVariables((prev) => ({ ...prev, [name]: value }));
  };

  const handleVariableTypeChange = (name, type) => {
    setVariableTypes((prev) => ({ ...prev, [name]: type }));
  };

  const handleFileSelect = async (name, filename) => {
    if (!filename) return;
    
    setFileLoading(prev => ({ ...prev, [name]: true }));
    try {
      const response = await axios.get(`${API_BASE_URL}/api/files/${filename}`, { 
        responseType: 'text',
        transformResponse: [(data) => data] // Prevent JSON parsing
      });
      handleVariableChange(name, response.data);
    } catch (error) {
      console.error('Failed to fetch file:', error);
      setError(`Failed to load file ${filename}: ${error.message}`);
    } finally {
      setFileLoading(prev => ({ ...prev, [name]: false }));
    }
  };

  const handleSendCommand = async () => {
    setIsLoading(true);
    try {
      let processedCommand = commandData;
      
      // Only process if there are variables to replace
      if (Object.keys(variables).length > 0) {
        for (const [key, value] of Object.entries(variables)) {
          const processedValue = variableTypes[key] === 'int' ? value : `"${value.replace(/^"|"$/g, '')}"`;
          processedCommand = processedCommand.replace(`"{{ ${key} }}"`, processedValue);
        }
      }
      
      await onSendCommand(processedCommand);
      setCommandData('');
      setError(null);
    } catch (err) {
      setError(err.message);
    } finally {
      setIsLoading(false);
    }
  };

  const handleTemplateSelect = (templateContent) => {
    setCommandData(templateContent);
    setActiveTab(0); // Switch back to command tab
  };

  const formatDate = (dateStr) => {
    try {
      const utcDate = new Date(dateStr + 'Z');
      return utcDate.toLocaleString();
    } catch (err) {
      return dateStr;
    }
  };

  return (
    <Box sx={{ p: 3 }}>
      {/* Top Section - Client Overview */}
      <Paper elevation={2} sx={{ p: 3, mb: 3 }}>
        <Typography variant="h5" gutterBottom>
          Liver Details
        </Typography>
        <Grid container spacing={3}>
          <Grid item xs={12} md={4}>
            <Typography variant="subtitle2" color="text.secondary">
              Liver ID
            </Typography>
            <Typography variant="body1" gutterBottom>
              {client.client_id}
            </Typography>
          </Grid>
          <Grid item xs={12} md={4}>
            <Typography variant="subtitle2" color="text.secondary">
              Last Connection
            </Typography>
            <Typography variant="body1" gutterBottom>
              {formatDate(client.last_connection)}
            </Typography>
          </Grid>
          <Grid item xs={12} md={4}>
            <Box>
              <Typography variant="subtitle2" color="text.secondary">
                Current IP
              </Typography>
              <Box sx={{ display: 'flex', alignItems: 'center' }}>
                <Typography variant="body1">{client.current_ip || client.ip || client.ip_history?.[0]?.ip || 'N/A'}</Typography>
              </Box>
            </Box>
          </Grid>
        </Grid>
        
        {/* IP History Section */}
        <Box sx={{ mt: 2 }}>
          <Box
            sx={{
              display: 'flex',
              alignItems: 'center',
              cursor: 'pointer'
            }}
            onClick={() => setIpHistoryExpanded(!ipHistoryExpanded)}
          >
            <Typography variant="subtitle1">
              IP History
            </Typography>
            <IconButton size="small">
              {ipHistoryExpanded ? <ExpandLessIcon /> : <ExpandMoreIcon />}
            </IconButton>
          </Box>
          <Collapse in={ipHistoryExpanded}>
            <List dense>
              {client.ip_history?.map((entry, index) => (
                <ListItem key={index}>
                  <ListItemText
                    primary={entry.ip}
                    secondary={formatDate(entry.last_seen)}
                  />
                </ListItem>
              ))}
            </List>
          </Collapse>
        </Box>
      </Paper>

      <Grid container spacing={3}>
        {/* Command Section */}
        <Grid item xs={12} md={8}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Send Command
            </Typography>
            <Box sx={{ borderBottom: 1, borderColor: 'divider', mb: 2 }}>
              <Tabs value={activeTab} onChange={(e, newValue) => setActiveTab(newValue)}>
                <Tab label="Command" />
                <Tab label="Templates" />
                <Tab label="Variables" />
              </Tabs>
            </Box>
            <Box sx={{ display: activeTab === 0 ? 'block' : 'none' }}>
              <Box sx={{ display: 'flex', gap: 2 }}>
                <TextField
                  fullWidth
                  multiline
                  rows={4}
                  variant="outlined"
                  placeholder="Enter command..."
                  value={commandData}
                  onChange={(e) => setCommandData(e.target.value)}
                  error={!!error}
                  helperText={error}
                />
                <LoadingButton
                  variant="contained"
                  onClick={handleSendCommand}
                  loading={isLoading}
                  disabled={!commandData}
                  endIcon={<SendIcon />}
                  sx={{ alignSelf: 'flex-start' }}
                >
                  Send
                </LoadingButton>
              </Box>
            </Box>
            <Box sx={{ display: activeTab === 1 ? 'block' : 'none', height: '200px' }}>
              <CommandTemplates onSelectTemplate={handleTemplateSelect} />
            </Box>
            <Box sx={{ display: activeTab === 2 ? 'block' : 'none', height: '200px' }}>
              <Paper variant="outlined" sx={{ p: 2, overflow: 'auto' }}>
                {Object.keys(variables).map((varName) => (
                  <Box key={varName} sx={{ mb: 2 }}>
                    <Typography variant="subtitle2" gutterBottom>
                      {varName}
                    </Typography>
                    <Box sx={{ display: 'flex', gap: 1 }}>
                      <TextField
                        fullWidth
                        variant="outlined"
                        value={variables[varName]}
                        onChange={(e) => handleVariableChange(varName, e.target.value)}
                        type={variableTypes[varName] === 'int' ? 'number' : 'text'}
                      />
                      <TextField
                        select
                        variant="outlined"
                        value={variableTypes[varName]}
                        onChange={(e) => handleVariableTypeChange(varName, e.target.value)}
                        sx={{ width: 100 }}
                      >
                        <MenuItem value="string">String</MenuItem>
                        <MenuItem value="int">Integer</MenuItem>
                      </TextField>
                      {files.length > 0 && (
                        <TextField
                          select
                          label="Load from file"
                          variant="outlined"
                          sx={{ minWidth: 150 }}
                          onChange={(e) => handleFileSelect(varName, e.target.value)}
                          disabled={fileLoading[varName]}
                        >
                          <MenuItem value="">
                            <em>Select a file...</em>
                          </MenuItem>
                          {files.map((file) => (
                            <MenuItem key={file} value={file}>
                              {file}
                            </MenuItem>
                          ))}
                        </TextField>
                      )}
                    </Box>
                  </Box>
                ))}
              </Paper>
            </Box>
          </Paper>
        </Grid>

        {/* Products Section */}
        <Grid item xs={12} md={4}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Products
            </Typography>
            <Grid container spacing={2}>
              {client.products?.map((productId, index) => {
                const product = client.parsed_products[productId];
                return (
                  <Grid item xs={12} key={index}>
                    <Card 
                      sx={{ 
                        cursor: 'pointer',
                        '&:hover': {
                          backgroundColor: 'action.hover'
                        }
                      }}
                      onClick={() => setSelectedProduct(productId)}
                    >
                      <CardContent>
                        <Typography variant="subtitle2" noWrap>
                          {product?.formatted_type || 'Unknown Type'}
                        </Typography>
                        <Typography variant="body2" color="text.secondary" noWrap>
                          {product?.id || 'Unknown ID'}
                        </Typography>
                      </CardContent>
                    </Card>
                  </Grid>
                );
              })}
            </Grid>
          </Paper>
        </Grid>
      </Grid>

      {/* Product Dialog */}
      <Dialog
        open={!!selectedProduct}
        onClose={() => setSelectedProduct(null)}
        maxWidth="md"
        fullWidth
      >
        <DialogContent>
          {selectedProduct && (
            <ProductViewer 
              product={client.parsed_products[selectedProduct]}
              productPath={client.product_paths[selectedProduct]}
            />
          )}
        </DialogContent>
      </Dialog>
    </Box>
  );
}
