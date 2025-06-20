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
  const [releases, setReleases] = useState([]);
  const [figs, setFigs] = useState([]);
  const [operation_types, setOperationTypes] = useState([]);

  useEffect(() => {
    const fetchReleases = async () => {
      try {
        const response = await axios.get(`${API_BASE_URL}/api/releases`);
        setReleases(response.data);
      } catch (error) {
        console.error('Failed to fetch releases:', error);
      }
    };
    fetchReleases();
  }, []);

  useEffect(() => {
    const fetchFigs = async () => {
      try {
        const response = await axios.get(`${API_BASE_URL}/api/figs`);
        setFigs(response.data);
      } catch (error) {
        console.error('Failed to fetch figs:', error);
      }
    };
    fetchFigs();
  }, []);

  useEffect(() => {
    const fetchOperationTypes = async () => {
      try {
        const response = await axios.get(`${API_BASE_URL}/api/operation-types`);
        setOperationTypes(response.data);
      } catch (error) {
        console.error('Failed to fetch operation types:', error);
      }
    };
    fetchOperationTypes();
  }, []);

  useEffect(() => {
    const extractedVariables = {};
    const extractedTypes = {};
    const regex = /{{[\s]*(\w+)[\s]*}}/g;
    let match;
    while ((match = regex.exec(commandData)) !== null) {
      extractedVariables[match[1]] = variables[match[1]] || '';
      extractedTypes[match[1]] = variableTypes[match[1]] || 'fig';
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

  const handleSendCommand = async () => {
    setIsLoading(true);
    try {
      let processedCommand = commandData;
      
      // Only process if there are variables to replace
      if (Object.keys(variables).length > 0) {
        for (const [key, value] of Object.entries(variables)) {
          const variableType = variableTypes[key]
          let processedValue = value

          if (variableType === 'string' || variableType === 'release') {
            processedValue = JSON.stringify(value)
          }

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

  const variableField = (varName) => {
    return (
      <>
        {variableTypes[varName] === 'release' ? (
          <TextField
            select
            fullWidth
            variant="outlined"
            onChange={(e) =>
              handleVariableChange(varName, { type: 'release', value: e.target.value })
            }
            size="small"
          >
            {(releases || []).map((release) => (
              <MenuItem key={release} value={release}>
                {release}
              </MenuItem>
            ))}
          </TextField>
        ) : (
            variableTypes[varName] === 'fig' ? (
              <TextField
                select
                fullWidth
                variant="outlined"
                onChange={(e) =>
                  handleVariableChange(varName, e.target.value)
                }
                size="small"
              >
                {(figs || []).map((fig) => (
                  <MenuItem key={fig.id} value={fig.id}>
                    {fig.name}
                  </MenuItem>
                ))}
              </TextField>
            ) : variableTypes[varName] === 'operation_type' ? (
              <TextField
                select
                fullWidth
                variant="outlined"
                onChange={(e) =>
                  handleVariableChange(varName, e.target.value)
                }
                size="small"
              >
                {(operation_types || []).map((operation_type) => (
                  <MenuItem key={operation_type.name} value={operation_type.value}>
                    {operation_type.name}
                  </MenuItem>
                ))}
              </TextField>
            ) : (
                <TextField
                  fullWidth
                  variant="outlined"
                  onChange={(e) => handleVariableChange(varName, e.target.value)}
                  type={variableTypes[varName] === 'int' ? 'number' : 'text'}
                  size="small"
                />
              )
        )}
        <TextField
          select
          variant="outlined"
          value={variableTypes[varName]}
          onChange={(e) => handleVariableTypeChange(varName, e.target.value)}
          sx={{ width: 120 }}
          size="small"
        >
          <MenuItem value="fig">Fig</MenuItem>
          <MenuItem value="operation_type">Operation Type</MenuItem>
          <MenuItem value="release">Release</MenuItem>
          <MenuItem value="string">String</MenuItem>
          <MenuItem value="int">Integer</MenuItem>
        </TextField>
      </>
    );
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
            <Typography variant="subtitle2" color="text.secondary">
              Location
            </Typography>
            <Typography variant="body1" gutterBottom>
              {client.location}
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
            {activeTab === 2 && Object.keys(variables).length > 0 && (
              <Paper 
                variant="outlined" 
                sx={{ 
                  p: 2,
                  overflow: 'auto',
                  '&::-webkit-scrollbar': {
                    width: '8px',
                  },
                  '&::-webkit-scrollbar-track': {
                    background: (theme) => theme.palette.mode === 'dark' ? '#333' : '#f1f1f1',
                  },
                  '&::-webkit-scrollbar-thumb': {
                    background: (theme) => theme.palette.mode === 'dark' ? '#666' : '#888',
                    borderRadius: '4px',
                  },
                  '&::-webkit-scrollbar-thumb:hover': {
                    background: (theme) => theme.palette.mode === 'dark' ? '#888' : '#555',
                  },
                }}
              >
                <Grid container spacing={2}>
                  {Object.keys(variables).map((varName) => (
                    <Grid item xs={12} key={varName}>
                      <Box sx={{ mb: 2 }}>
                        <Typography variant="subtitle2" gutterBottom>
                          {varName}
                        </Typography>
                        <Box sx={{ display: 'flex', gap: 1, flexWrap: 'nowrap' }}>
                          {variableField(varName)}
                        </Box>
                      </Box>
                    </Grid>
                  ))}
                </Grid>
              </Paper>
            )}
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
                        <Grid>
                          <Typography variant="body2" color="text.secondary" noWrap>
                            {product?.creation_time || 'Unknown Creation Time'}
                          </Typography>
                          <Typography variant="body2" color="text.secondary" noWrap>
                            {product?.id || 'Unknown ID'}
                          </Typography>
                        </Grid>
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
